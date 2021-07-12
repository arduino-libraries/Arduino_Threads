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
