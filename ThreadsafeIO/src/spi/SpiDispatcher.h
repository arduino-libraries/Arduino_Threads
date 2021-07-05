/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef SPI_DISPATCHER_H_
#define SPI_DISPATCHER_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>

#include "../IoResponse.h"

#include "SpiIoRequest.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class SpiDispatcher
{
public:

  SpiDispatcher(SpiDispatcher &) = delete;
  void operator = (SpiDispatcher &) = delete;

  static SpiDispatcher & instance();
  static void destroy();

  TSharedIoResponse dispatch(IoRequest * req);

private:

  static SpiDispatcher * _p_instance;
  static rtos::Mutex _mutex;

  rtos::Thread _thread;
  bool _has_tread_started;
  bool _terminate_thread;

  class IoTransaction
  {
  public:
    IoTransaction(IoRequest * q, IoResponse * p) : req{q}, rsp{p} { }
    IoRequest  * req{nullptr};
    IoResponse * rsp{nullptr};
  };
  
  static size_t constexpr REQUEST_QUEUE_SIZE = 32;
  rtos::Queue<IoTransaction, REQUEST_QUEUE_SIZE> _request_queue;

   SpiDispatcher();
  ~SpiDispatcher();

  void begin();
  void end();
  void threadFunc();
  void processSpiIoRequest(SpiIoRequest * spi_io_request);
};

#endif /* SPI_DISPATCHER_H_ */
