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
class Shared
{
  public:
    Shared() {
    }

    operator T()
    {
      T * val_ptr = _mailbox.try_get_for(rtos::Kernel::wait_for_u32_forever);
      if (val_ptr)
      {
        /* Copy the content of T stored in the memory pool since we'll have to free the memory pool afterwards. */
        T const tmp_val = *val_ptr;
        /* Free the allocated memory in the memory pool. */
        _mailbox.free(val_ptr);
        /* Return obtained value from _mailbox. */
        return tmp_val;
      }
      return val;
    }

    void operator = (T const & other)
    {
      if (_mailbox.full())
        T discard = *this; /* Invokes operator T() to discard oldest element and free its memory. */

      val = other;

      /* Try and allocate memory for the new entry in rtos::Mail. */
      T * val_ptr = _mailbox.try_alloc();
      if (val_ptr)
      {
        /* Copy the content of 'other' into the freshly allocated message. */
        *val_ptr = other;
        /* Copy the content of 'other' into the peek value. */
        val = other;
        /* Insert value into mailbox. */
        _mailbox.put(val_ptr);
      }
    }

    T& peek() {
      return val;
    }

    T& latest() {
      return peek();
    }

  private:

    T val;
    rtos::Mail<T, QUEUE_SIZE> _mailbox;
};

#endif /* ARDUINO_THREADS_SHARED_HPP_ */
