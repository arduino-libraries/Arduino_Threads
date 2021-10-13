/*
 * This file is part of the Arduino_ThreadsafeIO library.
 * Copyright (c) 2021 Arduino SA.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
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
