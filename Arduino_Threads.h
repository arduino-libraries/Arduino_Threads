template<class T>
class Shared // template definition
{
  public:
    Shared() {
      queue = new rtos::Queue<Shared<T>, 16>;
    }
    operator T() const {
      osEvent evt = queue->get();
      if (evt.status == osEventMessage) {
        Shared<T> *x = (Shared<T>*)evt.value.p;
        return x->val;
      }
    }
    T& operator= (const T& other) {
      val = other;
      queue->put(this);
    }
    T& peek() {
      return val;
    }
    T& latest() {
      return peek();
    }
  private:
    T val;
    rtos::Queue<Shared<T>, 16> *queue;
};

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

rtos::EventFlags ArduinoThreads::globalEvents;

#define THD_ENTER(tabname) class CONCAT(tabname, Class) : public ArduinoThreads { \
public: \
  CONCAT(tabname, Class)() { _tabname = _macroToString(tabname); } \
private: \

#define THD_DONE(tabname) \
};  \
CONCAT(tabname,Class) tabname;

#include "Wire.h"
#include "SerialDispatcher.h"