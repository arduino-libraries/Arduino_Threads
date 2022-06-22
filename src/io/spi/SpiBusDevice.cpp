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

#include "SpiBusDevice.h"

#include "SpiDispatcher.h"

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

SpiBusDevice::SpiBusDevice(SpiBusDeviceConfig const & config)
: _config{config}
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

IoResponse SpiBusDevice::transfer(IoRequest & req)
{
  return SpiDispatcher::instance().dispatch(&req, &_config);
}

bool SpiBusDevice::read(uint8_t * buffer, size_t len, uint8_t sendvalue)
{
  SpiBusDeviceConfig config(_config.spi(), _config.settings(), _config.selectFunc(), _config.deselectFunc(), sendvalue);
  IoRequest req(nullptr, 0, buffer, len);
  IoResponse rsp = SpiDispatcher::instance().dispatch(&req, &config);
  rsp->wait();
  return true;
}

bool SpiBusDevice::write(uint8_t * buffer, size_t len)
{
  IoRequest req(buffer, len, nullptr, 0);
  IoResponse rsp = SpiDispatcher::instance().dispatch(&req, &_config);
  rsp->wait();
  return true;
}

bool SpiBusDevice::writeThenRead(uint8_t * write_buffer, size_t write_len, uint8_t * read_buffer, size_t read_len, uint8_t sendvalue)
{
  SpiBusDeviceConfig config(_config.spi(), _config.settings(), _config.selectFunc(), _config.deselectFunc(), sendvalue);
  IoRequest req(write_buffer, write_len, read_buffer, read_len);
  IoResponse rsp = SpiDispatcher::instance().dispatch(&req, &config);
  rsp->wait();
  return true;
}
