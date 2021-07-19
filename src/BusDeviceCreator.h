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

#ifndef BUS_DEVICE_CREATOR_H_
#define BUS_DEVICE_CREATOR_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "spi/SpiBusDevice.h"
#include "wire/WireBusDevice.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

namespace impl
{

class BusDeviceCreator
{
public:

  SpiBusDevice  create(arduino::SPIClass & spi, int const cs_pin, SPISettings const & spi_settings, byte const fill_symbol = 0xFF);
  SpiBusDevice  create(arduino::SPIClass & spi, int const cs_pin, uint32_t const spi_clock, BitOrder const spi_bit_order, SPIMode const spi_bit_mode, byte const fill_symbol = 0xFF);
  SpiBusDevice  create(arduino::SPIClass & spi, SpiBusDeviceConfig::SpiSelectFunc spi_select, SpiBusDeviceConfig::SpiDeselectFunc spi_deselect, SPISettings const & spi_settings, byte const fill_symbol = 0xFF);

  WireBusDevice create(arduino::HardwareI2C & wire, byte const slave_addr);
  WireBusDevice create(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart);
  WireBusDevice create(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart, bool const stop);

};

} /* namespace impl */

/**************************************************************************************
 * EXTERN DECLARATION
 **************************************************************************************/

extern impl::BusDeviceCreator BusDeviceCreator;

#endif /* BUS_DEVICE_CREATOR_H_ */
