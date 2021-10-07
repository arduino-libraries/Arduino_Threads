#ifndef ARDUINO_THREADS_SHARED_HPP_
#define ARDUINO_THREADS_SHARED_HPP_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>
#include <MemoryPool.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

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

#endif /* ARDUINO_THREADS_SHARED_HPP_ */
