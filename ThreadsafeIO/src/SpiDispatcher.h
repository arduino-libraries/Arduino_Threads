/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef SPI_DISPATCHER_H_
#define SPI_DISPATCHER_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>

#include "IoRequest.h"

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

  bool request(IoRequest * req);

private:

  static SpiDispatcher * _p_instance;
  static rtos::Mutex _mutex;

  rtos::Thread _thread;
  bool _has_tread_started;
  bool _terminate_thread;

  static size_t constexpr REQUEST_QUEUE_SIZE = 32;
  rtos::Queue<IoRequest, REQUEST_QUEUE_SIZE> _request_queue;

   SpiDispatcher();
  ~SpiDispatcher();

  void begin();
  void end();
  void threadFunc();
  void processIoRequest(IoRequest * io_reqest);
};

#endif /* SPI_DISPATCHER_H_ */
