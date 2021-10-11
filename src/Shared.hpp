#ifndef ARDUINO_THREADS_SHARED_HPP_
#define ARDUINO_THREADS_SHARED_HPP_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

template<class T, size_t QUEUE_SIZE = 16>
class Shared
{
public:

  operator T();
  void operator = (T const & other);
  inline T peek() const { return _val; }


private:

  T _val;
  rtos::Mail<T, QUEUE_SIZE> _mailbox;
};

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

template<class T, size_t QUEUE_SIZE>
Shared<T,QUEUE_SIZE>::operator T()
{
  T * val_ptr = _mailbox.try_get_for(rtos::Kernel::wait_for_u32_forever);
  if (val_ptr)
  {
    T const tmp_val = *val_ptr;
    _mailbox.free(val_ptr);
    return tmp_val;
  }
  return _val;
}

template<class T, size_t QUEUE_SIZE>
void Shared<T,QUEUE_SIZE>::operator = (T const & other)
{
  /* If the mailbox is full we are discarding the
   * oldest element and then push the new one into
   * the queue.
   **/
  if (_mailbox.full())
  {
    T * val_ptr = _mailbox.try_get_for(rtos::Kernel::wait_for_u32_forever);
    _mailbox.free(val_ptr);
  }

  _val = other;

  T * val_ptr = _mailbox.try_alloc();
  if (val_ptr)
  {
    *val_ptr = other;
    _mailbox.put(val_ptr);
  }
}

#endif /* ARDUINO_THREADS_SHARED_HPP_ */
