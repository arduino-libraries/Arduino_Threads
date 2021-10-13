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

#ifndef IO_TRANSACTION_H_
#define IO_TRANSACTION_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino.h>

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

  IoRequest(byte * write_buf_, size_t const bytes_to_write_, byte * read_buf_, size_t const bytes_to_read_)
  : write_buf{write_buf_}
  , bytes_to_write{bytes_to_write_}
  , read_buf{read_buf_}
  , bytes_to_read{bytes_to_read_}
  { }

  IoRequest(byte & write_buf_, byte & read_buf_)
  : IoRequest{&write_buf_, 1, &read_buf_, 1}
  { }

  byte * write_buf{nullptr};
  size_t const bytes_to_write{0};
  byte * read_buf{nullptr};
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
