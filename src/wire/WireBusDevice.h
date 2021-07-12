/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef WIRE_BUS_DEVICE_H_
#define WIRE_BUS_DEVICE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino.h>
#include <mbed.h>

#include "../BusDevice.h"

#include "WireDispatcher.h"
#include "WireBusDeviceConfig.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class WireBusDevice : public BusDevice
{
public:

  WireBusDevice(std::string const & spi_bus, WireBusDeviceConfig const & config)
  : _config{config}
  { /* TODO: Select Wire bus based in string. */ }


  virtual IoResponse transfer(IoRequest & req) override
  {
    return WireDispatcher::instance().dispatch(&req, &_config);
  }


private:

  WireBusDeviceConfig _config;

};

#endif /* WIRE_BUS_DEVICE_H_ */
