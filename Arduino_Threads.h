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

#define THD_ENTER(tabname) class CONCAT(tabname, Class) { \

#define THD_DONE(tabname)   private: \
    void execute() {                 \
      setup();                       \
      while (1) {                    \
        loop();                      \
      }                              \
    }                                \
    rtos::Thread t;                  \
  public:                            \
    void start() {                   \
      t.start(mbed::callback(this, &CONCAT(tabname,Class)::execute));  \
    } \
    void begin() {                   \
      start();  \
    } \
};  \
CONCAT(tabname,Class) tabname;

/*
#define NEWTHREAD(tabname) THD_ENTER(tabname) \
                           \#include CONCAT(tabname, .h) \
                           THD_DONE(tabname)
*/

#include "Wire.h"
#include "SerialDispatcher.h"