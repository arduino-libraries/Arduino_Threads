/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "WireDispatcher.h"

#include <Wire.h>

/**************************************************************************************
 * STATIC MEMBER DEFINITION
 **************************************************************************************/

WireDispatcher * WireDispatcher::_p_instance{nullptr};
rtos::Mutex WireDispatcher::_mutex;

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

WireDispatcher::WireDispatcher()
: _thread(osPriorityRealtime, 4096, nullptr, "WireDispatcher")
, _has_tread_started{false}
, _terminate_thread{false}
{
  begin();
}

WireDispatcher::~WireDispatcher()
{
  end();
}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

WireDispatcher & WireDispatcher::instance()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  if (!_p_instance) {
    _p_instance = new WireDispatcher();
  }
  return *_p_instance;
}

void WireDispatcher::destroy()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  delete _p_instance;
  _p_instance = nullptr;
}

IoResponse WireDispatcher::dispatch(IoRequest * req, WireBusDeviceConfig * config)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);

  WireIoTransaction * wire_io_transaction = _wire_io_transaction_mailbox.try_alloc();
  if (!wire_io_transaction)
    return nullptr;

  IoResponse rsp(new impl::IoResponse());

  wire_io_transaction->req = req;
  wire_io_transaction->rsp = rsp;
  wire_io_transaction->config = config;

  _wire_io_transaction_mailbox.put(wire_io_transaction);

  return rsp;
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

void WireDispatcher::begin()
{
  Wire.begin();
  _thread.start(mbed::callback(this, &WireDispatcher::threadFunc)); /* TODO: Check return code */
  /* It is necessary to wait until the WireDispatcher::threadFunc()
   * has started, otherwise other threads might trigger IO requests
   * before this thread is actually running.
   */
  while (!_has_tread_started) { }
}

void WireDispatcher::end()
{
  _terminate_thread = true;
  _thread.join(); /* TODO: Check return code */
  Wire.end();
}

void WireDispatcher::threadFunc()
{
  _has_tread_started = true;

  while(!_terminate_thread)
  {
    /* Wait blocking for the next IO transaction
     * request to be posted to the mailbox.
     */
    WireIoTransaction * wire_io_transaction = _wire_io_transaction_mailbox.try_get_for(rtos::Kernel::wait_for_u32_forever);
    if (wire_io_transaction)
    {
      processWireIoRequest(wire_io_transaction);
      /* Free the allocated memory (memory allocated
       * during dispatch(...)
       */
      _wire_io_transaction_mailbox.free(wire_io_transaction);
    }
  }
}

void WireDispatcher::processWireIoRequest(WireIoTransaction * wire_io_transaction)
{
  IoRequest           * io_request  = wire_io_transaction->req;
  IoResponse            io_response = wire_io_transaction->rsp;
  WireBusDeviceConfig * config      = wire_io_transaction->config;

  Wire.beginTransmission(config->slave_addr());

  size_t bytes_written = 0;
  for (; bytes_written < io_request->bytes_to_write; bytes_written++)
  {
    Wire.write(io_request->write_buf[bytes_written]);
  }
  io_response->bytes_written = bytes_written;

  if (config->restart() && (io_request->bytes_to_read > 0))
    Wire.endTransmission(false /* stop */);
  else
    Wire.endTransmission(true /* stop */);

  if (io_request->bytes_to_read > 0)
  {
    Wire.requestFrom(config->slave_addr(), io_request->bytes_to_read, config->stop());

    while(Wire.available() != static_cast<int>(io_request->bytes_to_read))
    {
      /* TODO: Insert a timeout. */
    }

    size_t bytes_read = 0;
    for (; bytes_read < io_request->bytes_to_read; bytes_read++)
    {
      io_request->read_buf[bytes_read] = Wire.read();
    }
    io_response->bytes_read = bytes_read;
  }

  io_response->done();
}
