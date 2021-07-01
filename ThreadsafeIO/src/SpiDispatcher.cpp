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

rtos::Queue<IoRequest, 32> _request_queue;
