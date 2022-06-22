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

#ifndef WIRE_BUS_DEVICE_H_
#define WIRE_BUS_DEVICE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "../BusDevice.h"

#include "WireBusDeviceConfig.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class WireBusDevice : public BusDeviceBase
{
public:

           WireBusDevice(WireBusDeviceConfig const & config);
  virtual ~WireBusDevice() { }


  virtual IoResponse transfer(IoRequest & req) override;


  bool read(uint8_t * buffer, size_t len, bool stop = true);
  bool write(uint8_t * buffer, size_t len, bool stop = true);
  bool writeThenRead(uint8_t * write_buffer, size_t write_len, uint8_t * read_buffer, size_t read_len, bool stop = false);


private:

  WireBusDeviceConfig _config;

};

#endif /* WIRE_BUS_DEVICE_H_ */
