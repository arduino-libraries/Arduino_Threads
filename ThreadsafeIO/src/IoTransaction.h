/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef IO_TRANSACTION_H_
#define IO_TRANSACTION_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <stdint.h>

#include <mbed.h>
#include <SharedPtr.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

/**************************************************************************************
 * IoRequest
 **************************************************************************************/

class IoRequest
{
public:

  IoRequest(uint8_t const * const write_buf_, size_t const bytes_to_write_, uint8_t * read_buf_, size_t const bytes_to_read_)
  : write_buf{write_buf_}
  , bytes_to_write{bytes_to_write_}
  , read_buf{read_buf_}
  , bytes_to_read{bytes_to_read_}
  { }


  uint8_t const * const write_buf{nullptr};
  size_t const bytes_to_write{0};
  uint8_t * read_buf{nullptr};
  size_t const bytes_to_read{0};

};

/**************************************************************************************
 * IoResponse
 **************************************************************************************/

namespace impl
{

class IoResponse
{
public:

  IoResponse()
  : bytes_written{0}
  , bytes_read{0}
  , _cond{_mutex}
  , _is_done{false}
  { }

  size_t bytes_written{0};
  size_t bytes_read{0};

  void done()
  {
    _mutex.lock();
    _is_done = true;
    _cond.notify_all();
    _mutex.unlock();
  }

  void wait()
  {
    _mutex.lock();
    while (!_is_done) {
      _cond.wait();
    }
    _mutex.unlock();
  }

private:

  rtos::Mutex _mutex;
  rtos::ConditionVariable _cond;
  bool _is_done{false};

};

} /* namespace impl */

typedef mbed::SharedPtr<impl::IoResponse> IoResponse;

/**************************************************************************************
 * IoTransaction
 **************************************************************************************/

class IoTransaction
{
public:

  IoTransaction(IoRequest * q, IoResponse * p) : req{q}, rsp{p} { }
  IoRequest  * req{nullptr};
  IoResponse * rsp{nullptr};
};

#endif /* IO_TRANSACTION_H_ */
