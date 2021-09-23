#ifndef ARDUINO_THREADS_H_
#define ARDUINO_THREADS_H_

#include <mbed.h>
#include <MemoryPool.h>

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

// Forward declaration of Sink and Source
template<class T>
class Sink;
template<class T>
class Source;

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

template<class T>
class Source
{
  public:
    Source() {};

    void connectTo(Sink<T> &sink) {
      if (destination == nullptr) {
        destination = &sink;
      } else {
        destination->connectTo(sink);
      }
    }

    void send(const T &value) {
      if (destination) destination->inject(value);
    }

  private:
    Sink<T> *destination;
};

template<class T, size_t QUEUE_SIZE = 16>
class Shared // template definition
{
  public:
    Shared() {
    }
    operator T() {
      osEvent evt = queue.get();
      if (evt.status == osEventMessage) {
        /* Obtain the oldest inserted element from the queue. */
        T * val_ptr = reinterpret_cast<T *>(evt.value.p);
        /* Copy the content of T stored in the memory pool since we'll have to free the memory pool afterwards. */
        T const tmp_val = *val_ptr;
        /* Free the allocated memory in the memory pool. */
        memory_pool.free(val_ptr);
        /* Return obtained value from queue. */
        return tmp_val;
      }
      return val;
    }
    T& operator= (const T& other) {
      if (queue.full()) {
        // invokes operator T() to discard oldest element and free its memory
        T discard = *this;
      }
      val = other;
      /* Allocate memory in the memory pool. */
      T * val_ptr = memory_pool.alloc();
      /* Copy the content of 'other' into the freshly allocated message. */
      *val_ptr = other;
      /* Insert into queue. */
      queue.put(val_ptr);
      return (*val_ptr);
    }
    T& peek() {
      return val;
    }
    T& latest() {
      return peek();
    }
  private:

    T val;
    rtos::MemoryPool<T, QUEUE_SIZE> memory_pool;
    rtos::Queue<T, QUEUE_SIZE> queue;
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

#endif /* ARDUINO_THREADS_H_ */
