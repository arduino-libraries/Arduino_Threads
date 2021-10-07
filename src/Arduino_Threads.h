#ifndef ARDUINO_THREADS_H_
#define ARDUINO_THREADS_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>

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

#define SINK(name, type, size) \
public: \
  Sink<type> name{size}; \
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

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

#define CONCAT2(x,y) x##y
#define CONCAT(x,y) CONCAT2(x,y)

#define INCF(F) INCF_(F)
#define INCF_(F) #F

#define _macroToString(sequence) #sequence

class ArduinoThreads
{
public:

  void start       (int const stack_size = 4096, uint32_t const start_flags = 0, uint32_t const stop_flags = 0);
  void terminate   ();
  void setLoopDelay(uint32_t const delay);
  void sendEvent   (uint32_t const event);

  static void broadcastEvent(uint32_t event);


protected:

  char * _tabname;


private:

  static rtos::EventFlags _global_events;
  rtos::Thread *t;
  uint32_t _start_flags, _stop_flags;
  uint32_t _loop_delay;

  virtual void setup(void) {};
  virtual void loop(void) {};

  void execute();
};

#define THD_ENTER(tabname) class CONCAT(tabname, Class) : public ArduinoThreads { \
public: \
  CONCAT(tabname, Class)() { _tabname = _macroToString(tabname); } \
private: \

#define THD_DONE(tabname) \
};  \
CONCAT(tabname,Class) tabname;

#endif /* ARDUINO_THREADS_H_ */
