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

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "WireBusDevice.h"

#include "WireDispatcher.h"

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

WireBusDevice::WireBusDevice(WireBusDeviceConfig const & config)
: _config{config}
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

IoResponse WireBusDevice::transfer(IoRequest & req)
{
  return WireDispatcher::instance().dispatch(&req, &_config);
}

bool WireBusDevice::read(uint8_t * buffer, size_t len, bool stop)
{
  WireBusDeviceConfig config(_config.wire(), _config.slaveAddr(), _config.restart(), stop);
  IoRequest req(nullptr, 0, buffer, len);
  IoResponse rsp = WireDispatcher::instance().dispatch(&req, &config);
  rsp->wait();
  return true;
}

bool WireBusDevice::write(uint8_t * buffer, size_t len, bool stop)
{
  bool const restart = !stop;
  WireBusDeviceConfig config(_config.wire(), _config.slaveAddr(), restart, _config.stop());
  IoRequest req(buffer, len, nullptr, 0);
  IoResponse rsp = WireDispatcher::instance().dispatch(&req, &config);
  rsp->wait();
  return true;
}

bool WireBusDevice::writeThenRead(uint8_t * write_buffer, size_t write_len, uint8_t * read_buffer, size_t read_len, bool stop)
{
  /* Copy the Wire parameters from the device and modify only those
   * which can be modified via the parameters of this function.
   */
  bool const restart = !stop;
  WireBusDeviceConfig config(_config.wire(), _config.slaveAddr(), restart, _config.stop());
  /* Fire off the IO request and await its response. */
  IoRequest req(write_buffer, write_len, read_buffer, read_len);
  IoResponse rsp = WireDispatcher::instance().dispatch(&req, &config);
  rsp->wait();
  /* TODO: Introduce error codes within the IoResponse and evaluate
   * them here.
   */
  return true;
}
