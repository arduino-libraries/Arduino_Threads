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

#include "BusDeviceCreator.h"

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

namespace impl
{

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

SpiBusDevice BusDeviceCreator::create(arduino::SPIClass & spi, SPISettings const & spi_settings, SpiBusDeviceConfig::SpiSelectFunc spi_select, SpiBusDeviceConfig::SpiDeselectFunc spi_deselect, byte const fill_symbol)
{
  return SpiBusDevice(SpiBusDeviceConfig{spi, spi_settings, spi_select, spi_deselect, fill_symbol});
}

SpiBusDevice BusDeviceCreator::create(arduino::SPIClass & spi, SPISettings const & spi_settings, int const cs_pin, byte const fill_symbol)
{
  return SpiBusDevice(SpiBusDeviceConfig{spi, spi_settings, cs_pin, fill_symbol});
}

WireBusDevice BusDeviceCreator::create(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart, bool const stop)
{
  return WireBusDevice(WireBusDeviceConfig{wire, slave_addr, restart, stop});
}

WireBusDevice BusDeviceCreator::create(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart)
{
  return create(wire, slave_addr, restart, true);
}

WireBusDevice BusDeviceCreator::create(arduino::HardwareI2C & wire, byte const slave_addr)
{
  return create(wire, slave_addr, true, true);
}

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* namespace impl */

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

impl::BusDeviceCreator BusDeviceCreator;
