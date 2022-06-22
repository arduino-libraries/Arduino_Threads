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

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "SpiDispatcher.h"

#include <SPI.h>

/**************************************************************************************
 * STATIC MEMBER DEFINITION
 **************************************************************************************/

SpiDispatcher * SpiDispatcher::_p_instance{nullptr};
rtos::Mutex SpiDispatcher::_mutex;

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

SpiDispatcher::SpiDispatcher()
: _thread(osPriorityRealtime, 4096, nullptr, "SpiDispatcher")
, _has_tread_started{false}
, _terminate_thread{false}
{
  begin();
}

SpiDispatcher::~SpiDispatcher()
{
  end();
}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

SpiDispatcher & SpiDispatcher::instance()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  if (!_p_instance) {
    _p_instance = new SpiDispatcher();
  }
  return *_p_instance;
}

void SpiDispatcher::destroy()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  delete _p_instance;
  _p_instance = nullptr;
}

IoResponse SpiDispatcher::dispatch(IoRequest * req, SpiBusDeviceConfig * config)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);

  SpiIoTransaction * spi_io_transaction = _spi_io_transaction_mailbox.try_alloc();
  if (!spi_io_transaction)
    return nullptr;

  IoResponse rsp(new impl::IoResponse());

  spi_io_transaction->req = req;
  spi_io_transaction->rsp = rsp;
  spi_io_transaction->config = config;

  _spi_io_transaction_mailbox.put(spi_io_transaction);

  return rsp;
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

void SpiDispatcher::begin()
{
  SPI.begin();
  _thread.start(mbed::callback(this, &SpiDispatcher::threadFunc)); /* TODO: Check return code */
  /* It is necessary to wait until the SpiDispatcher::threadFunc()
   * has started, otherwise other threads might trigger IO requests
   * before this thread is actually running.
   */
  while (!_has_tread_started) { }
}

void SpiDispatcher::end()
{
  _terminate_thread = true;
  _thread.join(); /* TODO: Check return code */
  SPI.end();
}

void SpiDispatcher::threadFunc()
{
  _has_tread_started = true;

  while(!_terminate_thread)
  {
    /* Wait blocking for the next IO transaction
     * request to be posted to the mailbox.
     */
    SpiIoTransaction * spi_io_transaction = _spi_io_transaction_mailbox.try_get_for(rtos::Kernel::wait_for_u32_forever);
    if (spi_io_transaction)
    {
      processSpiIoRequest(spi_io_transaction);
      /* Free the allocated memory (memory allocated
       * during dispatch(...)
       */
      _spi_io_transaction_mailbox.free(spi_io_transaction);
    }
  }
}

void SpiDispatcher::processSpiIoRequest(SpiIoTransaction * spi_io_transaction)
{
  IoRequest          * io_request  = spi_io_transaction->req;
  IoResponse           io_response = spi_io_transaction->rsp;
  SpiBusDeviceConfig * config      = spi_io_transaction->config;

  config->select();

  config->spi().beginTransaction(config->settings());

  /* In a first step transmit the complete write buffer and
   * write back the receive data directly into the write buffer  
   */
  size_t bytes_sent = 0;
  for(; bytes_sent < io_request->bytes_to_write; bytes_sent++)
  {
    uint8_t const tx_byte = io_request->write_buf[bytes_sent];
    uint8_t const rx_byte = config->spi().transfer(tx_byte);

    io_request->write_buf[bytes_sent] = rx_byte;
  }

  /* In a second step, transmit the fill symbol and write the
   * received data into the read buffer.
   */
  size_t bytes_received = 0;
  for(; bytes_received < io_request->bytes_to_read; bytes_received++)
  {
    uint8_t const tx_byte = config->fillSymbol();
    uint8_t const rx_byte = config->spi().transfer(tx_byte);

    io_request->read_buf[bytes_received] = rx_byte;
  }

  config->spi().endTransaction();

  config->deselect();

  io_response->bytes_written = bytes_sent;
  io_response->bytes_read = bytes_received;

  io_response->done();
}
