/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef SPI_BUS_DEVICE_H_
#define SPI_BUS_DEVICE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <functional>

#include <Arduino.h>
#include <mbed.h>
#include <SPI.h>

#include "IoRequest.h"
#include "BusDevice.h"

#include "SpiDispatcher.h"
#include "SpiBusDeviceConfig.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class SpiIoRequest : public IoRequest
{
public:
  SpiIoRequest(uint8_t const * const tx_buf, size_t const tx_buf_len, uint8_t * rx_buf, size_t const rx_buf_len, size_t * bytes_read)
  : IoRequest(IoRequest::Type::SPI, tx_buf, tx_buf_len, rx_buf, rx_buf_len, bytes_read)
  { }

  void set_config(SpiBusDeviceConfig * config) { _config = config; }
  SpiBusDeviceConfig & config() { return *_config; }

private:
  SpiBusDeviceConfig * _config{nullptr};
};

class SpiBusDevice : public BusDevice
{
public:
  SpiBusDevice(std::string const & spi_bus, SpiBusDeviceConfig const & config) : _config{config}
  {
    /* TODO: Select SPI bus based in string. */
  }
  virtual Status transfer(IoRequest & req) override
  {
    reinterpret_cast<SpiIoRequest*>(&req)->set_config(&_config);

    /* Insert into queue. */
    SpiDispatcher::instance().request(&req);

    return Status::Ok;
  }

private:

  SpiBusDeviceConfig _config;

};

#endif /* SPI_BUS_DEVICE_H_ */