/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef IO_RESPONSE_H_
#define IO_RESPONSE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <stdint.h>

#include <mbed.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class IoResponse
{
public:

  class ReadBuf
  {
  public:
    ReadBuf(uint8_t * d, size_t * br) : data{d}, bytes_read{br} { }
    uint8_t * data{nullptr};
    size_t * bytes_read{0};
  };


  IoResponse(uint8_t * rx_buf, size_t * bytes_read)
  : _read_buf{rx_buf, bytes_read}
  , _cond{_mutex}
  { }

  inline ReadBuf  & read_buf ()       { return _read_buf; }

  rtos::Mutex _mutex;
  rtos::ConditionVariable _cond;

private:

  ReadBuf _read_buf;

};

#endif /* IO_RESPONSE_H_ */