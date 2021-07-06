/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef WIRE_BUS_DEVICE_CONFIG_H_
#define WIRE_BUS_DEVICE_CONFIG_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class WireBusDeviceConfig
{
public:

  WireBusDeviceConfig(byte const slave_addr, bool const restart = true, bool const stop = true)
  : _slave_addr{slave_addr}
  , _restart{restart}
  , _stop{stop}
  { }


  inline byte slave_addr() const { return _slave_addr; }
  inline bool restart()    const { return _restart; }
  inline bool stop()       const { return _stop; }


private:

  byte _slave_addr{0x00};
  bool _restart{true}, _stop{true};

};

#endif /* WIRE_BUS_DEVICE_CONFIG_H_ */
