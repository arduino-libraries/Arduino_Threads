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

#ifndef SPI_DISPATCHER_H_
#define SPI_DISPATCHER_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <mbed.h>

#include "../IoTransaction.h"

#include "SpiBusDeviceConfig.h"

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

  IoResponse dispatch(IoRequest * req, SpiBusDeviceConfig * config);

private:

  static SpiDispatcher * _p_instance;
  static rtos::Mutex _mutex;

  rtos::Thread _thread;
  bool _has_tread_started;
  bool _terminate_thread;

  typedef struct
  {
    IoRequest  * req;
    IoResponse rsp;
    SpiBusDeviceConfig * config;
  } SpiIoTransaction;

  static size_t constexpr REQUEST_QUEUE_SIZE = 32;
  rtos::Mail<SpiIoTransaction, REQUEST_QUEUE_SIZE> _spi_io_transaction_mailbox;

   SpiDispatcher();
  ~SpiDispatcher();

  void begin();
  void end();
  void threadFunc();
  void processSpiIoRequest(SpiIoTransaction * spi_io_transaction);
};

#endif /* SPI_DISPATCHER_H_ */
