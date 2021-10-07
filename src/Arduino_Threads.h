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

class ArduinoThreads {
  private:
    static rtos::EventFlags globalEvents;
    uint32_t startFlags;
    uint32_t stopFlags;
    uint32_t loopDelay;
    virtual void setup(void) {};
    virtual void loop(void) {};
    void execute() {
      setup();
      // if startFlags have been passed then wait until all the flags are set
      // before starting the loop. this is used to synchronize loops from multiple
      // sketches.
      if (startFlags != 0) {
        globalEvents.wait_all(startFlags);
      }

      // if stopFlags have been passed stop when all the flags are set
      // otherwise loop forever
      while ( 1 ) {
        loop();
        // on exit clear the flags that have forced us to stop.
        // note that if two groups of sketches stop on common flags
        // the first group will clear them so the second group may never
        // exit
        if (stopFlags!=0) {
          if ((globalEvents.get()&stopFlags)!=stopFlags) {
            globalEvents.clear(stopFlags);
            return;
          }
          if ((rtos::ThisThread::flags_get()&stopFlags)!=stopFlags) {
            rtos::ThisThread::flags_clear(stopFlags);
            return;
          }
        }
        // sleep for the time we've been asked to insert between loops 
        rtos::ThisThread::sleep_for(loopDelay);
      }
    }
    rtos::Thread *t;

  protected:
    char* _tabname;

  public:
    // start this sketch
    void start(int stacksize = 4096, uint32_t startFlags=0, uint32_t stopFlags=0) {
      this->startFlags = startFlags;
      this->stopFlags = stopFlags;
      loopDelay=0;
      t = new rtos::Thread(osPriorityNormal, stacksize, nullptr, _tabname);
      t->start(mbed::callback(this, &ArduinoThreads::execute));
    }
    // kill this sketch
    void terminate() {
      t->terminate();
    }
    // send an event to all sketches at the same time
    static void broadcastEvent(uint32_t event) {
      globalEvents.set(event);
    }
    // send an event only to this sketch
    void sendEvent(uint32_t event) {
      t->flags_set(event);
    }
    // set the rate at which loop function will be called
    void setLoopDelay(uint32_t delay) {
      loopDelay = delay;
    }
};

#define THD_ENTER(tabname) class CONCAT(tabname, Class) : public ArduinoThreads { \
public: \
  CONCAT(tabname, Class)() { _tabname = _macroToString(tabname); } \
private: \

#define THD_DONE(tabname) \
};  \
CONCAT(tabname,Class) tabname;

#endif /* ARDUINO_THREADS_H_ */
