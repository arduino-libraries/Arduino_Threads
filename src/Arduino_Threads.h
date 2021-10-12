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

#ifndef ARDUINO_THREADS_H_
#define ARDUINO_THREADS_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>
#include <SharedPtr.h>

#include "Sink.hpp"
#include "Source.hpp"
#include "Shared.hpp"

/**************************************************************************************
 * DEFINE
 **************************************************************************************/

#define SOURCE(name, type) \
public: \
  Source<type> name; \
private:

#define SINK(name, type) \
public: \
  SinkBlocking<type> name; \
private:
// we need to call the Sink<T>(int size) non-default constructor using size as parameter.
// This is done by writing
//    Sink<type> name{size};
// instead of:
//    Sink<type> name(size);
// otherwise the compiler will read it as a declaration of a method called "name" and we
// get a syntax error.
// This is called "C++11 uniform init" (using "{}" instead of "()" without "="... yikes!)
// https://chromium.googlesource.com/chromium/src/+/master/styleguide/c++/c++-dos-and-donts.md

#define SHARED(name, type) \
  Shared<type> name;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

#define CONCAT2(x,y) x##y
#define CONCAT(x,y) CONCAT2(x,y)

#define INCF(F) INCF_(F)
#define INCF_(F) #F

#define _macroToString(sequence) #sequence

class Arduino_Threads
{
public:

           Arduino_Threads();
  virtual ~Arduino_Threads();


  void start       (int const stack_size = 4096, uint32_t const start_flags = 0, uint32_t const stop_flags = 0);
  void terminate   ();
  void setLoopDelay(uint32_t const delay);
  void sendEvent   (uint32_t const event);

  static void broadcastEvent(uint32_t event);


protected:

  char * _tabname;

  virtual void setup() = 0;
  virtual void loop () = 0;

private:

  static rtos::EventFlags _global_events;
  mbed::SharedPtr<rtos::Thread> _thread;
  uint32_t _start_flags, _stop_flags;
  uint32_t _loop_delay_ms;

  void threadFunc();
};

#define THD_ENTER(tabname) class CONCAT(tabname, Class) : public Arduino_Threads { \
public: \
  CONCAT(tabname, Class)() { _tabname = _macroToString(tabname); } \
private: \

#define THD_DONE(tabname) \
};  \
CONCAT(tabname,Class) tabname;

#endif /* ARDUINO_THREADS_H_ */
