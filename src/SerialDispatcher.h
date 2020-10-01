#ifndef __SERIAL_DISPATCHER_H__
#define __SERIAL_DISPATCHER_H__

#include "Arduino.h"
#undef Serial
#include "api/RingBuffer.h"

struct _sinkBuffers {
  osThreadId_t id;
  bool reader;
  bool raw;
  rtos::Semaphore* sem;
  RingBuffer rxBuffer;
  RingBuffer txBuffer;
};

#define READ_READY_UNBLOCK          (1 << 1)

class SerialClassDispatcher : public HardwareSerial {
  public:
    SerialClassDispatcher(HardwareSerial& _serial) : serial(_serial) {
      
    }
    void begin(unsigned long baudrate) {
      if (!begun) {
        serial.begin(baudrate);
        begun = true;
        printer.start(mbed::callback(this, &SerialClassDispatcher::timedPrint));
      }
      sinkBuffers[users].id = rtos::ThisThread::get_id();
      sinkBuffers[users].sem = new rtos::Semaphore(1);
      sinkBuffers[users].raw = false;
      users++;
    }

    void begin(unsigned long baudrate, uint16_t config) {
      if (!begun) {
        serial.begin(baudrate, config);
        begun = true;
      }
      sinkBuffers[users].id = rtos::ThisThread::get_id();
      sinkBuffers[users].sem = new rtos::Semaphore(1);
      sinkBuffers[users].raw = false;
      users++;
    }

    using Print::write; // pull in write(str) and write(buf, size) from Print

    operator bool() {
      return serial;
    }

    void tags(bool enable = true) {
      print_tags = enable;
    }

    void end() {
      if (users == 0) {
        serial.end();
      } else {
        users--;
      }
    }

    size_t write(uint8_t data) {
      findSemaphore(rtos::ThisThread::get_id())->acquire();
      findThreadTxBuffer(rtos::ThisThread::get_id()).store_char(data);
      findSemaphore(rtos::ThisThread::get_id())->release();
      unlock_print.set(READ_READY_UNBLOCK);
    }

    size_t write(const uint8_t* data, size_t len) {
      findSemaphore(rtos::ThisThread::get_id())->acquire();
      for (int i=0; i<len; i++) {
        findThreadTxBuffer(rtos::ThisThread::get_id()).store_char(data[i]);
      }
      findSemaphore(rtos::ThisThread::get_id())->release();
      unlock_print.set(READ_READY_UNBLOCK);
    }

    void flushReadBuffer() {
      while (serial.available()) {
        int c = serial.read();
        for (int i = 0; i < users; i++) {
          if (sinkBuffers[i].reader) {
            sinkBuffers[i].rxBuffer.store_char(c);
          }
        }
      }
    }

    int read() {
      *isReader(rtos::ThisThread::get_id()) = true;
      flushReadBuffer();
      return findThreadRxBuffer(rtos::ThisThread::get_id()).read_char();
    }

    int peek() {
      *isReader(rtos::ThisThread::get_id()) = true;
      flushReadBuffer();
      return findThreadRxBuffer(rtos::ThisThread::get_id()).peek();
    }

    void raw(bool _raw = true) {
      *isRaw(rtos::ThisThread::get_id()) = _raw;
    }

    void flush() {
      serial.flush();
    }

    int available() {
      *isReader(rtos::ThisThread::get_id()) = true;
      flushReadBuffer();
      return findThreadRxBuffer(rtos::ThisThread::get_id()).available();
    }

  private:

    void timedPrint() {
      while (1) {
        unlock_print.wait_any(READ_READY_UNBLOCK, osWaitForever, true);
        for (int i = 0; i < users; i++) {
          sinkBuffers[i].sem->acquire();
          // Implementation "leak", should be changed at RingBuffer API level
          int c = sinkBuffers[i].txBuffer._iHead == 0 ?
            sinkBuffers[i].txBuffer._aucBuffer[sizeof(sinkBuffers[i].txBuffer._aucBuffer) -1] :
            sinkBuffers[i].txBuffer._aucBuffer[sinkBuffers[i].txBuffer._iHead - 1];
          if ((!sinkBuffers[i].raw && (c == '\n' /*|| c == '\r' */|| c == '\0')) ||
              sinkBuffers[i].raw || !sinkBuffers[i].txBuffer.availableForStore()) {
            if (sinkBuffers[i].txBuffer.available() && print_tags) {
              serial.print("[");
              serial.print(i);
              serial.print("] ");
            }
            while (sinkBuffers[i].txBuffer.available()) {
              serial.write(sinkBuffers[i].txBuffer.read_char());
            }
          }
          sinkBuffers[i].sem->release();
        }
      }
    }

    bool* isReader(osThreadId_t id) {
      for (int i = 0; i < 10; i++) {
        if (id == sinkBuffers[i].id) {
          return &sinkBuffers[i].reader;
        }
      }
    }

    bool* isRaw(osThreadId_t id) {
      for (int i = 0; i < 10; i++) {
        if (id == sinkBuffers[i].id) {
          return &sinkBuffers[i].raw;
        }
      }
    }

    rtos::Semaphore* findSemaphore(osThreadId_t id) {
      for (int i = 0; i < 10; i++) {
        if (id == sinkBuffers[i].id) {
          return sinkBuffers[i].sem;
        }
      }
    }

    RingBuffer& findThreadTxBuffer(osThreadId_t id) {
      for (int i = 0; i < 10; i++) {
        if (id == sinkBuffers[i].id) {
          return sinkBuffers[i].txBuffer;
        }
      }
    }

    RingBuffer& findThreadRxBuffer(osThreadId_t id) {
      for (int i = 0; i < 10; i++) {
        if (id == sinkBuffers[i].id) {
          return sinkBuffers[i].rxBuffer;
        }
      }
    }

  private:
    HardwareSerial& serial;
    int users = 0;
    bool begun = false;
    int currentClock = 400000;
    struct _sinkBuffers sinkBuffers[10];
    osThreadId_t currentThread;
    rtos::Thread printer;
    rtos::EventFlags unlock_print;
    bool print_tags = false;
};

extern SerialClassDispatcher Serial;

#endif