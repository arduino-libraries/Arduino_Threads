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

#ifndef ARDUINO_THREADS_SHARED_HPP_
#define ARDUINO_THREADS_SHARED_HPP_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>

/**************************************************************************************
 * CONSTANT
 **************************************************************************************/

static size_t constexpr SHARED_QUEUE_SIZE = 16;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

template<class T, size_t QUEUE_SIZE = SHARED_QUEUE_SIZE>
class Shared
{
public:

  T pop();
  void push(T const & val);
  inline T peek() const { return _val; }

private:

  T _val;
  rtos::Mail<T, QUEUE_SIZE> _mailbox;

};

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

template<class T, size_t QUEUE_SIZE>
T Shared<T,QUEUE_SIZE>::pop()
{
  T * val_ptr = _mailbox.try_get_for(rtos::Kernel::wait_for_u32_forever);
  if (val_ptr)
  {
    T const tmp_val = *val_ptr;
    _mailbox.free(val_ptr);
    return tmp_val;
  }
  return _val;
}

template<class T, size_t QUEUE_SIZE>
void Shared<T,QUEUE_SIZE>::push(T const & val)
{
  /* If the mailbox is full we are discarding the
   * oldest element and then push the new one into
   * the queue.
   **/
  if (_mailbox.full())
  {
    T * val_ptr = _mailbox.try_get_for(rtos::Kernel::wait_for_u32_forever);
    _mailbox.free(val_ptr);
  }

  _val = val;

  T * val_ptr = _mailbox.try_alloc();
  if (val_ptr)
  {
    *val_ptr = val;
    _mailbox.put(val_ptr);
  }
}

#endif /* ARDUINO_THREADS_SHARED_HPP_ */
