/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "SpiDispatcher.h"

#include <SPI.h>

#include "SpiBusDevice.h"

/**************************************************************************************
 * STATIC MEMBER DEFINITION
 **************************************************************************************/

SpiDispatcher * SpiDispatcher::_p_instance{nullptr};
rtos::Mutex SpiDispatcher::_mutex;

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

SpiDispatcher::SpiDispatcher()
: _thread(osPriorityNormal, 4096, nullptr, "SpiDispatcher")
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

IoResponse * SpiDispatcher::request(IoRequest * req)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  /* ATTENTION!!! MEM LEAK HERE!!! */
  IoResponse * rsp = new IoResponse{req->read_buf().data, req->read_buf().bytes_read};
  IoTransaction * io_transaction = new IoTransaction(req, rsp);
  if (_request_queue.try_put(io_transaction)) {
    return rsp;
  } else {
    return nullptr;
  }
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

void SpiDispatcher::begin()
{
  SPI.begin();
  _thread.start(mbed::callback(this, &SpiDispatcher::threadFunc)); /* TODO: Check return code */
  /* Is is necessary to wait until the SpiDispatcher::threadFunc()
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
    IoTransaction * io_transaction = nullptr;
    if (_request_queue.try_get(&io_transaction))
    {
      IoRequest * io_reqest = io_transaction->req;
      IoResponse * io_response = io_transaction->rsp;
      if (io_reqest->type() == IoRequest::Type::SPI)
      {
        SpiIoRequest * spi_io_request = reinterpret_cast<SpiIoRequest *>(io_reqest);

        io_response->_mutex.lock();
        processSpiIoRequest(spi_io_request);
        io_response->_cond.notify_all();
        io_response->_mutex.unlock();
      }
    }
  }
}

void SpiDispatcher::processSpiIoRequest(SpiIoRequest * spi_io_request)
{
  spi_io_request->config().select();

  SPI.beginTransaction(spi_io_request->config().settings());

  size_t bytes_received = 0,
         bytes_sent = 0;
  for(;
      bytes_received < spi_io_request->read_buf().bytes_to_read;
      bytes_received++, bytes_sent++)
  {
    uint8_t tx_byte = 0;

    if (bytes_sent < spi_io_request->write_buf().bytes_to_write)
      tx_byte = spi_io_request->write_buf().data[bytes_sent];
    else
      tx_byte = spi_io_request->config().fill_symbol();

    uint8_t const rx_byte = SPI.transfer(tx_byte);

    Serial.print("TX ");
    Serial.print(tx_byte, HEX);
    Serial.print("| RX ");
    Serial.print(rx_byte, HEX);
    Serial.println();

    spi_io_request->read_buf().data[bytes_received] = rx_byte;
  }
  *spi_io_request->read_buf().bytes_read = bytes_received;

  SPI.endTransaction();

  spi_io_request->config().deselect();
}
