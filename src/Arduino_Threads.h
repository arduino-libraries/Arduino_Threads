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

#include "threading/Sink.hpp"
#include "threading/Source.hpp"
#include "threading/Shared.hpp"

#include "io/BusDevice.h"
#include "io/util/util.h"
#include "io/spi/SpiBusDevice.h"
#include "io/wire/WireBusDevice.h"
#include "io/serial/SerialDispatcher.h"

/**************************************************************************************
 * DEFINE
 **************************************************************************************/

#define SOURCE(name, type) \
Source<type> name;

/* We need to call the SinkBlocking<T>(size_t const size)
 * non-default constructor using size as parameter.

 * This is achieved via
 *   SinkBlocking<type> name{size};
 * instead of
 *   SinkBlocking<type> name(size);
 * otherwise the compiler will read it as a declaration
 * of a method called "name" and we get a syntax error.
 *
 * This is called "C++11 uniform init" (using "{}" instead
 * of "()" without "="... yikes!)
 *   https://chromium.googlesource.com/chromium/src/+/master/styleguide/c++/c++-dos-and-donts.md
 */

#define SINK_2_ARG(name, type) \
SinkBlocking<type> name{1}

#define SINK_3_ARG(name, type, size) \
SinkBlocking<type> name{size}

/* Black C macro magic enabling "macro overloading"
 * with same name macro, but multiple arguments.
 * https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
 */
#define GET_SINK_MACRO(_1,_2,_3,NAME,...) NAME
#define SINK(...) GET_SINK_MACRO(__VA_ARGS__, SINK_3_ARG, SINK_2_ARG)(__VA_ARGS__)

#define SINK_NON_BLOCKING(name, type) \
SinkNonBlocking<type> name{}

#define CONNECT(source_thread, source_name, sink_thread, sink_name) \
source_thread##Private::source_name.connectTo(sink_thread##Private::sink_name)

#define SHARED_2_ARG(name, type) \
  Shared<type> name;

#define SHARED_3_ARG(name, type, size) \
  Shared<type, size> name;

#define GET_SHARED_MACRO(_1,_2,_3,NAME,...) NAME
#define SHARED(...) GET_SHARED_MACRO(__VA_ARGS__, SHARED_3_ARG, SHARED_2_ARG)(__VA_ARGS__)


#define ARDUINO_THREADS_CONCAT_(x,y) x##y
#define ARDUINO_THREADS_CONCAT(x,y) ARDUINO_THREADS_CONCAT_(x,y)

#define ARDUINO_THREADS_TO_STRING(sequence) #sequence

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

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

#define THD_SETUP(ns) ns::setup()
#define THD_LOOP(ns) ns::loop()

#define THD_ENTER(tabname) \
namespace ARDUINO_THREADS_CONCAT(tabname,Private)\
{\
  void setup();\
  void loop();\
}\
class ARDUINO_THREADS_CONCAT(tabname, Class) : public Arduino_Threads\
{\
public:\
  ARDUINO_THREADS_CONCAT(tabname, Class)() { _tabname = ARDUINO_THREADS_TO_STRING(tabname); }\
protected:\
  virtual void setup() override { THD_SETUP(ARDUINO_THREADS_CONCAT(tabname,Private)); }\
  virtual void loop() override { THD_LOOP(ARDUINO_THREADS_CONCAT(tabname,Private)); }\
};\
namespace ARDUINO_THREADS_CONCAT(tabname,Private)\
{

#define THD_DONE(tabname)\
};\
ARDUINO_THREADS_CONCAT(tabname,Class) tabname;

#endif /* ARDUINO_THREADS_H_ */
