/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef SPI_DISPATCHER_H_
#define SPI_DISPATCHER_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino.h>
#include <mbed.h>
#include <SPI.h>

#include "IoRequest.h"
#include "SpiBusDevice.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

extern rtos::Queue<IoRequest, 32> _request_queue;

class SpiDispatcher
{
public:

   SpiDispatcher() : _thread(osPriorityNormal, 4096, nullptr, "SpiDispatcher") { }
  ~SpiDispatcher() { end(); }

  enum class Status : int
  {
    Ok = 0
  };

  Status begin()
  {
    SPI.begin();
    _thread.start(mbed::callback(this, &SpiDispatcher::threadFunc)); /* TODO: Check return code */
    return Status::Ok;
  }

  Status end()
  {
    _terminate_thread = true;
    _thread.join(); /* TODO: Check return code */
    SPI.end();
    return Status::Ok;
  }

private:

  rtos::Thread _thread;
  bool _terminate_thread{false};

  void threadFunc()
  {
    while(!_terminate_thread)
    {
      //rtos::ThisThread::yield();
      IoRequest * io_reqest = nullptr;
      if (_request_queue.try_get(&io_reqest))
      {
        if (io_reqest->_type == IoRequest::Type::SPI)
        {
          SpiIoRequest * spi_io_request = reinterpret_cast<SpiIoRequest *>(io_reqest);

          spi_io_request->config().select();

          SPI.beginTransaction(spi_io_request->config().settings());

          size_t bytes_received = 0,
                 bytes_sent = 0;
          for(; bytes_received < *(spi_io_request->_rx_buf_len); bytes_received++, bytes_sent++)
          {
            uint8_t tx_byte = 0;

            if (bytes_sent < spi_io_request->_tx_buf_len)
                tx_byte = spi_io_request->_tx_buf[bytes_sent];
            else
                tx_byte = spi_io_request->config().fill_symbol();

            uint8_t const rx_byte = SPI.transfer(tx_byte);

            Serial.print("TX ");
            Serial.print(tx_byte, HEX);
            Serial.print("| RX ");
            Serial.print(rx_byte, HEX);
            Serial.println();

            spi_io_request->_rx_buf[bytes_received] = rx_byte;
          }
          *spi_io_request->_rx_buf_len = bytes_received;

          SPI.endTransaction();

          spi_io_request->config().deselect();
        }
      }
    }
  }
};

#endif /* SPI_DISPATCHER_H_ */
