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
    rtos::EventFlags *GlobalEvents;
    rtos::EventFlags ThreadEvents;
    virtual void setup(void) {};
    virtual void loop(void) {};
    void execute() {                 
      setup();
      if (GlobalEvents!=NULL) GlobalEvents->wait_all(LoopStart);                       
      while ( (GlobalEvents==NULL || !(GlobalEvents->get()&LoopStop) ) &&
             !(ThreadEvents.get()&LoopStop) ) {                    
        loop();                      
      }                              
    }                                
    rtos::Thread t;                  
  public:
    typedef enum {
      LoopStart= (1<<0),
      LoopStop=  (1<<1)
    } Events;
    void start(rtos::EventFlags *event = NULL) {
      GlobalEvents = event;
      t.start(mbed::callback(this, &ArduinoThreads::execute));  
    } 
    void terminate() {
      t.terminate();
    }
    void stop() {
      ThreadEvents.set(LoopStop);
    }
};

#define THD_ENTER(tabname) class CONCAT(tabname, Class) : public ArduinoThreads { \
private:

#define THD_DONE(tabname) \
};  \
CONCAT(tabname,Class) tabname;

#include "Wire.h"
#include "SerialDispatcher.h"