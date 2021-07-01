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

class SpiBusDevice : public BusDevice
{
public:
  SpiBusDevice(std::string const & spi_bus, SpiBusDeviceConfig const & config) : _config{config}
  {
    /* TODO: Select SPI bus based in string. */
  }
  virtual bool transfer(IoRequest & req) override
  {
    /* Append SPI bus device specific configuration. */
    reinterpret_cast<SpiIoRequest*>(&req)->set_config(&_config);
    /* Dispatch the request into the queue. */
    return SpiDispatcher::instance().request(&req);
  }

private:

  SpiBusDeviceConfig _config;

};

#endif /* SPI_BUS_DEVICE_H_ */
