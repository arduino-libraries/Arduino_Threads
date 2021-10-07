/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "Arduino_Threads.h"

/**************************************************************************************
 * STATIC MEMBER DECLARATION
 **************************************************************************************/

rtos::EventFlags ArduinoThreads::_global_events;

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

ArduinoThreads::ArduinoThreads()
: _start_flags{0}
, _stop_flags{0}
, _loop_delay{0}
{

}

ArduinoThreads::~ArduinoThreads()
{
  terminate();
}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

void ArduinoThreads::start(int const stack_size, uint32_t const start_flags, uint32_t const stop_flags)
{
  _start_flags = start_flags;
  _stop_flags  = stop_flags;
  _thread.reset(new rtos::Thread(osPriorityNormal, stack_size, nullptr, _tabname));
  _thread->start(mbed::callback(this, &ArduinoThreads::threadFunc));
}

void ArduinoThreads::terminate()
{
  _thread->terminate();
  _thread->join();
}

void ArduinoThreads::sendEvent(uint32_t const event)
{
  _thread->flags_set(event);
}

void ArduinoThreads::setLoopDelay(uint32_t const delay)
{
  _loop_delay = delay;
}

void ArduinoThreads::broadcastEvent(uint32_t const event)
{
  _global_events.set(event);
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

void ArduinoThreads::threadFunc()
{
  setup();
  /* If _start_flags have been passed then wait until all the flags are set
   * before starting the loop. this is used to synchronize loops from multiple
   * sketches.
   */
  if (_start_flags != 0)
    _global_events.wait_all(_start_flags);

  /* if _stop_flags have been passed stop when all the flags are set
   * otherwise loop forever
   */
  for (;;)
  {
    loop();
    /* On exit clear the flags that have forced us to stop.
     * note that if two groups of sketches stop on common flags
     * the first group will clear them so the second group may never
     * exit.
     */
    if (_stop_flags!=0)
    {
      if ((_global_events.get() & _stop_flags) != _stop_flags)
      {
        _global_events.clear(_stop_flags);
        return;
      }

      if ((rtos::ThisThread::flags_get() & _stop_flags) != _stop_flags)
      {
        rtos::ThisThread::flags_clear(_stop_flags);
        return;
      }
    }

    /* Sleep for the time we've been asked to insert between loops.
     */
    rtos::ThisThread::sleep_for(_loop_delay);
  }
}
