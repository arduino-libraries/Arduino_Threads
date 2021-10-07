#ifndef ARDUINO_THREADS_SINK_HPP_
#define ARDUINO_THREADS_SINK_HPP_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

template<class T>
class Sink
{
  private:
    rtos::Mutex dataMutex;
    rtos::ConditionVariable dataAvailable;
    rtos::ConditionVariable slotAvailable;
    T latest;
    Sink *next;
    const int size;
    int first, last;
    bool full;
    T *queue;

  public:
    Sink(int s) :
      dataAvailable(dataMutex),
      slotAvailable(dataMutex),
      size(s),
      queue((size > 0) ? new T[size] : nullptr),
      first(0), last(0), full(false)
    {};

    ~Sink() {
      if (queue != nullptr) { delete queue; }
    }


  //protected: TODO
    void connectTo(Sink &sink)
    {
      if (next == nullptr) {
        next = &sink;
      } else {
        next->connectTo(sink);
      }
    }

    T read()
    {
      // Non-blocking shared variable
      if (size == -1) {
        dataMutex.lock();
        T res = latest;
        dataMutex.unlock();
        return res;
      }

      // Blocking shared variable
      if (size == 0) {
        dataMutex.lock();
        while (!full) {
          dataAvailable.wait();
        }
        T res = latest;
        full = false;
        slotAvailable.notify_all();
        dataMutex.unlock();
        return res;
      }

      // Blocking queue
      dataMutex.lock();
      while (first == last && !full) {
        dataAvailable.wait();
      }
      T res = queue[first++];
      first %= size;
      if (full) {
        full = false;
        slotAvailable.notify_one();
      }
      dataMutex.unlock();
      return res;
    }

  //protected: TODO
    void inject(const T &value)
    {
      dataMutex.lock();

      // Non-blocking shared variable
      if (size == -1) {
        latest = value;
      }

      // Blocking shared variable
      else if (size == 0) {
        while (full) {
          slotAvailable.wait();
        }
        latest = value;
        full = true;
        dataAvailable.notify_one();
        slotAvailable.wait();
      }

      // Blocking queue
      else {
        while (full) {
          slotAvailable.wait();
        }
        if (first == last) {
          dataAvailable.notify_one();
        }
        queue[last++] = value;
        last %= size;
        if (first == last) {
          full = true;
        }
      }
      dataMutex.unlock();

      if (next) next->inject(value);
    }
};

#endif /* ARDUINO_THREADS_SINK_HPP_ */
