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

#ifndef ARDUINO_THREADS_RINGBUFFER_HPP_
#define ARDUINO_THREADS_RINGBUFFER_HPP_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <SharedPtr.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

template <typename T>
class CircularBuffer
{
public:

  CircularBuffer(size_t const size);

  void store(T const data);
  T read();
  bool isFull() const;
  bool isEmpty() const;


private:

  mbed::SharedPtr<T> _data;
  size_t const _size;
  size_t _head, _tail, _num_elems;

  size_t next(size_t const idx);
};

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

template <typename T>
CircularBuffer<T>::CircularBuffer(size_t const size)
: _data{new T[size]}
, _size{size}
, _head{0}
, _tail{0}
, _num_elems{0}
{
}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

template <typename T>
void CircularBuffer<T>::store(T const data)
{
  if (!isFull())
  {
    _data.get()[_head] = data;
    _head = next(_head);
    _num_elems++;
  }
}

template <typename T>
T CircularBuffer<T>::read()
{
  if (isEmpty())
    return T{0};

  T const value = _data.get()[_tail];
  _tail = next(_tail);
  _num_elems--;

  return value;
}

template <typename T>
bool CircularBuffer<T>::isFull() const
{
  return (_num_elems == _size);
}

template <typename T>
bool CircularBuffer<T>::isEmpty() const
{
  return (_num_elems == 0);
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

template <typename T>
size_t CircularBuffer<T>::next(size_t const idx)
{
  return ((idx + 1) % _size);
}

#endif /* ARDUINO_THREADS_RINGBUFFER_HPP_ */
