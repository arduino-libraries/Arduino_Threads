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

#ifndef ARDUINO_THREADS_SINK_HPP_
#define ARDUINO_THREADS_SINK_HPP_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

template<class T>
class Sink
{
  private:
    rtos::Mutex dataMutex;
    rtos::ConditionVariable dataAvailable;
    rtos::ConditionVariable slotAvailable;
    T latest;
    Sink *next;
    const int size;
    int first, last;
    bool full;
    T *queue;

  public:
    Sink(int s) :
      dataAvailable(dataMutex),
      slotAvailable(dataMutex),
      size(s),
      queue((size > 0) ? new T[size] : nullptr),
      first(0), last(0), full(false)
    {};

    ~Sink() {
      if (queue != nullptr) { delete queue; }
    }


  //protected: TODO
    void connectTo(Sink &sink)
    {
      if (next == nullptr) {
        next = &sink;
      } else {
        next->connectTo(sink);
      }
    }

    T read()
    {
      // Non-blocking shared variable
      if (size == -1) {
        dataMutex.lock();
        T res = latest;
        dataMutex.unlock();
        return res;
      }

      // Blocking shared variable
      if (size == 0) {
        dataMutex.lock();
        while (!full) {
          dataAvailable.wait();
        }
        T res = latest;
        full = false;
        slotAvailable.notify_all();
        dataMutex.unlock();
        return res;
      }

      // Blocking queue
      dataMutex.lock();
      while (first == last && !full) {
        dataAvailable.wait();
      }
      T res = queue[first++];
      first %= size;
      if (full) {
        full = false;
        slotAvailable.notify_one();
      }
      dataMutex.unlock();
      return res;
    }

  //protected: TODO
    void inject(const T &value)
    {
      dataMutex.lock();

      // Non-blocking shared variable
      if (size == -1) {
        latest = value;
      }

      // Blocking shared variable
      else if (size == 0) {
        while (full) {
          slotAvailable.wait();
        }
        latest = value;
        full = true;
        dataAvailable.notify_one();
        slotAvailable.wait();
      }

      // Blocking queue
      else {
        while (full) {
          slotAvailable.wait();
        }
        if (first == last) {
          dataAvailable.notify_one();
        }
        queue[last++] = value;
        last %= size;
        if (first == last) {
          full = true;
        }
      }
      dataMutex.unlock();

      if (next) next->inject(value);
    }
};

#endif /* ARDUINO_THREADS_SINK_HPP_ */
