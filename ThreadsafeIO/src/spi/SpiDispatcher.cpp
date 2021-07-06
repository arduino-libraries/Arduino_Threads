/*
 * A deeply magical library providing threadsafe IO via pipes.
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

IoResponse SpiDispatcher::dispatch(IoRequest * req, SpiBusDeviceConfig * config)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);

  SpiIoTransaction * spi_io_transaction = _spi_io_transaction_mailbox.try_alloc();
  if (!spi_io_transaction)
    return nullptr;

  IoResponse rsp(new impl::IoResponse{req->read_buf});

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
    /* Wait blocking for the next IO transaction
     * request to be posted to the mailbox.
     */
    osEvent evt = _spi_io_transaction_mailbox.get();
    if (evt.status == osEventMail)
    {
      /* Fetch the IO transaction request and
       * process it.
       */
      SpiIoTransaction * spi_io_transaction = reinterpret_cast<SpiIoTransaction *>(evt.value.p);
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

  io_response->_mutex.lock();

  config->select();

  SPI.beginTransaction(config->settings());

  size_t bytes_received = 0,
         bytes_sent = 0;
  for(;
      bytes_received < io_request->bytes_to_read;
      bytes_received++, bytes_sent++)
  {
    uint8_t tx_byte = 0;

    if (bytes_sent < io_request->bytes_to_write)
      tx_byte = io_request->write_buf[bytes_sent];
    else
      tx_byte = config->fill_symbol();

    uint8_t const rx_byte = SPI.transfer(tx_byte);
/*
    Serial.print("TX ");
    Serial.print(tx_byte, HEX);
    Serial.print("| RX ");
    Serial.print(rx_byte, HEX);
    Serial.println();
*/
    io_request->read_buf[bytes_received] = rx_byte;
  }
  SPI.endTransaction();

  config->deselect();

  io_response->bytes_written = bytes_sent;
  io_response->bytes_read = bytes_received;

  io_response->_cond.notify_all();
  io_response->_mutex.unlock();
}
