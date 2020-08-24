#ifndef __WIRE_WRAPPER_H__
#define __WIRE_WRAPPER_H__

#include "Portenta_System.h" // just a trick to allow including the real Wire.h
#define Wire WireReal
#define Wire1 WireReal1
#include "../Wire/Wire.h"
#undef Wire
#undef Wire1
#include "api/RingBuffer.h"

struct requiredClocks {
  osThreadId_t id;
  int clock;
  RingBuffer rxBuffer;
  bool transaction;
};

class WireClassDispatcher : public HardwareI2C {
  public:
    WireClassDispatcher(HardwareI2C& _wire) : wire(_wire) {
      sem = new rtos::Semaphore(1);
    }
    void begin() {
      sem->acquire();
      if (!begun) {
        wire.begin();
        begun = true;
      }
      idClock[users].id = rtos::ThisThread::get_id();
      idClock[users].clock = currentClock;
      users++;
      sem->release();
    }
    void begin(uint8_t) {
      /*doNothing*/
    }
    void onReceive(void (*)(int)) {
      /*doNothing*/
    }
    void onRequest(void (*)()) {
      /*doNothing*/
    }

    void end() {
      if (users == 0) {
        wire.end();
      } else {
        users--;
      }
    }

    void setClock(uint32_t freq) {
      // must be called on a per-thread basis
      if (freq != currentClock) {
        wire.setClock(freq);
        currentClock = freq;
        *findThreadClock(rtos::ThisThread::get_id()) = currentClock;
      }
    }

    void beginTransmission(uint8_t address) {
      //lock to caller thread until endTransmission(true) is called
      sem->acquire();
      int freq = *findThreadClock(rtos::ThisThread::get_id());
      if (freq != currentClock) {
        setClock(freq);
      }
      currentThread = rtos::ThisThread::get_id();
      wire.beginTransmission(address);
    }

    uint8_t endTransmission(bool stopBit) {
      uint8_t res = wire.endTransmission(stopBit);
      if (stopBit) {
        *transactionInProgress(rtos::ThisThread::get_id()) = false;
        sem->release();
      } else {
        *transactionInProgress(rtos::ThisThread::get_id()) = true;
      }
      return res;
    }

    uint8_t endTransmission(void) {
      return endTransmission(true);
    }

    uint8_t requestFrom(uint8_t address, size_t len, bool stopBit) {
      if (!transactionInProgress(rtos::ThisThread::get_id())) {
        sem->acquire();
      }
      uint8_t ret = wire.requestFrom(address, len, stopBit);
      if (ret > 0) {
        while (wire.available()) {
          findThreadRxBuffer(rtos::ThisThread::get_id()).store_char(wire.read());
        }
      }
      if (stopBit) {
        *transactionInProgress(rtos::ThisThread::get_id()) = false;
        sem->release();
      } else {
        *transactionInProgress(rtos::ThisThread::get_id()) = true;
      }
      return ret;
    }

    uint8_t requestFrom(uint8_t address, size_t len) {
      return requestFrom(address, len, true);
    }

    size_t write(uint8_t data) {
      if (currentThread != rtos::ThisThread::get_id()) {
        return 0;
      }
      return wire.write(data);
    }

    size_t write(const uint8_t* data, int len) {
      if (currentThread != rtos::ThisThread::get_id()) {
        return 0;
      }
      return wire.write(data, len);
    }

    int read() {
      return findThreadRxBuffer(rtos::ThisThread::get_id()).read_char();
    }

    int peek() {
      return findThreadRxBuffer(rtos::ThisThread::get_id()).peek();
    }

    void flush() {
    }

    int available() {
      return findThreadRxBuffer(rtos::ThisThread::get_id()).available();
    }

  private:
    int* findThreadClock(osThreadId_t id) {
      for (int i = 0; i < 10; i++) {
        if (id == idClock[i].id) {
          return &idClock[i].clock;
        }
      }
    }
    bool* transactionInProgress(osThreadId_t id) {
      for (int i = 0; i < 10; i++) {
        if (id == idClock[i].id) {
          return &idClock[i].transaction;
        }
      }
    }
    RingBuffer& findThreadRxBuffer(osThreadId_t id) {
      for (int i = 0; i < 10; i++) {
        if (id == idClock[i].id) {
          return idClock[i].rxBuffer;
        }
      }
    }

  private:
    HardwareI2C& wire;
    int users = 0;
    bool begun = false;
    rtos::Semaphore* sem;
    int currentClock = 400000;
    struct requiredClocks idClock[10];
    osThreadId_t currentThread;
};

extern WireClassDispatcher Wire;
extern WireClassDispatcher Wire1;

#define TwoWire WireClassDispatcher

#endif
