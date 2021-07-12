/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef WIRE_DISPATCHER_H_
#define WIRE_DISPATCHER_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>

#include "../IoTransaction.h"

#include "WireBusDeviceConfig.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class WireDispatcher
{
public:

  WireDispatcher(WireDispatcher &) = delete;
  void operator = (WireDispatcher &) = delete;

  static WireDispatcher & instance();
  static void destroy();

  IoResponse dispatch(IoRequest * req, WireBusDeviceConfig * config);

private:

  static WireDispatcher * _p_instance;
  static rtos::Mutex _mutex;

  rtos::Thread _thread;
  bool _has_tread_started;
  bool _terminate_thread;

  typedef struct
  {
    IoRequest  * req;
    IoResponse rsp;
    WireBusDeviceConfig * config;
  } WireIoTransaction;

  static size_t constexpr REQUEST_QUEUE_SIZE = 32;
  rtos::Mail<WireIoTransaction, REQUEST_QUEUE_SIZE> _wire_io_transaction_mailbox;

   WireDispatcher();
  ~WireDispatcher();

  void begin();
  void end();
  void threadFunc();
  void processWireIoRequest(WireIoTransaction * wire_io_transaction);
};

#endif /* WIRE_DISPATCHER_H_ */
