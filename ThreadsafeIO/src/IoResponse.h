/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef IO_RESPONSE_H_
#define IO_RESPONSE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <stdint.h>

#include <mbed.h>
#include <SharedPtr.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class IoResponse
{
public:

  IoResponse(uint8_t * read_buf_)
  : _cond{_mutex}
  , read_buf{read_buf_}
  , bytes_written{0}
  , bytes_read{0}
  { }

  rtos::Mutex _mutex;
  rtos::ConditionVariable _cond;
  uint8_t * read_buf{nullptr};
  size_t bytes_written{0};
  size_t bytes_read{0};

  void wait()
  {
    _mutex.lock(); /* Do we really need to lock/unlock the mutex? */
    _cond.wait();
    _mutex.unlock();
  }

};

typedef mbed::SharedPtr<IoResponse> TSharedIoResponse;

#endif /* IO_RESPONSE_H_ */