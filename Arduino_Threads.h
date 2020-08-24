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

class ArduinoThreadClass {
public:
  void add(rtos::Thread* me) {
    if (controller == NULL) {
        controller = new rtos::Thread(osPriorityHigh, 512);
        controller->start(mbed::callback(this, &ArduinoThreadClass::loop));
    }
    list[idx++] = me;
  }

  void ping(rtos::Thread* me) {
    for (int i=0; i < idx; i++) {
      if (me == list[i]) {
        timestamps[i] = millis();
      }
    }
  }

private:

  void loop() {
    while (1) {
      for (int i = 0; i < idx; i++) {
        if (millis() - timestamps[i] > 5000) {
          list[i]->terminate();
          // TODO: reorder threads and make list[i] == NULL
          Serial.println(list[i]->get_name() + String(" killed "));
        }
      }
      delay(1000);
    }
  }

  rtos::Thread* controller = NULL;
  rtos::Thread* list[10] = {NULL};
  uint32_t timestamps[10] = {0};
  int idx = 0;
};

ArduinoThreadClass ArduinoThread;

#define _macroToString(sequence) #sequence

#define THD_ENTER(tabname) class CONCAT(tabname, Class) { \

#define THD_DONE(tabname)   private: \
    void execute() {                 \
      ArduinoThread.add(t);          \
      setup();                       \
      while (1) {                    \
        loop();                      \
        ArduinoThread.ping(t);       \
      }                              \
    }                                \
    rtos::Thread* t;                 \
                                     \
  public:                            \
    void start(int stacksize = 4096) {            \
      t = new rtos::Thread(osPriorityNormal, stacksize, nullptr, _macroToString(tabname)); \
      t->start(mbed::callback(this, &CONCAT(tabname,Class)::execute));  \
    }                                \
    void begin() {                   \
      start();                       \
    }                                \
};                                   \
CONCAT(tabname,Class) tabname;

/*
#define NEWTHREAD(tabname) THD_ENTER(tabname) \
                           \#include CONCAT(tabname, .h) \
                           THD_DONE(tabname)
*/

#include "Wire.h"
#include "SerialDispatcher.h"