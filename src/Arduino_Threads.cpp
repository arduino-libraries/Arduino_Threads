/*
 * This file is part of the Arduino_ThreadsafeIO library.
 * Copyright (c) 2021 Arduino SA.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "Arduino_Threads.h"

/**************************************************************************************
 * STATIC MEMBER DECLARATION
 **************************************************************************************/

rtos::EventFlags Arduino_Threads::_global_events;

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

Arduino_Threads::Arduino_Threads()
: _start_flags{0}
, _stop_flags{0}
, _loop_delay_ms{0}
{

}

Arduino_Threads::~Arduino_Threads()
{
  terminate();
}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

void Arduino_Threads::start(int const stack_size, uint32_t const start_flags, uint32_t const stop_flags)
{
  _start_flags = start_flags;
  _stop_flags  = stop_flags;
  _thread.reset(new rtos::Thread(osPriorityNormal, stack_size, nullptr, _tabname));
  _thread->start(mbed::callback(this, &Arduino_Threads::threadFunc));
}

void Arduino_Threads::terminate()
{
  _thread->terminate();
  _thread->join();
}

void Arduino_Threads::sendEvent(uint32_t const event)
{
  _thread->flags_set(event);
}

void Arduino_Threads::setLoopDelay(uint32_t const delay)
{
  _loop_delay_ms = delay;
}

void Arduino_Threads::broadcastEvent(uint32_t const event)
{
  _global_events.set(event);
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

void Arduino_Threads::threadFunc()
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
    rtos::ThisThread::sleep_for(rtos::Kernel::Clock::duration_u32(_loop_delay_ms));
  }
}
