/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "SpiDispatcher.h"

/**************************************************************************************
 * STATIC MEMBER DEFINITION
 **************************************************************************************/

SpiDispatcher * SpiDispatcher::_p_instance{nullptr};
rtos::Mutex SpiDispatcher::_mutex;

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

SpiDispatcher::SpiDispatcher()
: _thread(osPriorityNormal, 4096, nullptr, "SpiDispatcher")
, _has_tread_started{false}
, _terminate_thread{false}
{
  begin();
}

SpiDispatcher::~SpiDispatcher()
{
  end();
}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

SpiDispatcher & SpiDispatcher::instance()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  if (!_p_instance) {
    _p_instance = new SpiDispatcher();
  }
  return *_p_instance;
}

void SpiDispatcher::destroy()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  delete _p_instance;
  _p_instance = nullptr;
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

void SpiDispatcher::begin()
{
  SPI.begin();
  _thread.start(mbed::callback(this, &SpiDispatcher::threadFunc)); /* TODO: Check return code */
  /* Is is necessary to wait until the SpiDispatcher::threadFunc()
   * has started, otherwise other threads might trigger IO requests
   * before this thread is actually running.
   */
  while (!_has_tread_started) { }
}


void SpiDispatcher::end()
{
  _terminate_thread = true;
  _thread.join(); /* TODO: Check return code */
  SPI.end();
}


rtos::Queue<IoRequest, 32> _request_queue;
