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

#ifndef ARDUINO_THREADS_SOURCE_HPP_
#define ARDUINO_THREADS_SOURCE_HPP_

/**************************************************************************************
 * FORWARD DECLARATION
 **************************************************************************************/

template<class T>
class Sink;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

template<class T>
class Source
{
  public:
    Source() {};

    void connectTo(Sink<T> &sink) {
      if (destination == nullptr) {
        destination = &sink;
      } else {
        destination->connectTo(sink);
      }
    }

    void send(const T &value) {
      if (destination) destination->inject(value);
    }

  private:
    Sink<T> *destination;
};

#endif /* ARDUINO_THREADS_SOURCE_HPP_ */
