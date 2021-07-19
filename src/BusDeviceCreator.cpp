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

SpiBusDevice BusDeviceCreator::create(std::string const & spi_bus, SPISettings const & spi_settings, SpiBusDeviceConfig::SpiSelectFunc spi_select, SpiBusDeviceConfig::SpiDeselectFunc spi_deselect, byte const fill_symbol)
{
  return SpiBusDevice(spi_bus, SpiBusDeviceConfig{spi_settings, spi_select, spi_deselect, fill_symbol});
}

SpiBusDevice BusDeviceCreator::create(std::string const & spi_bus, SPISettings const & spi_settings, int const cs_pin, byte const fill_symbol)
{
  return SpiBusDevice(spi_bus, SpiBusDeviceConfig{spi_settings, cs_pin, fill_symbol});
}

/**************************************************************************************
 * NAMESPACE
 **************************************************************************************/

} /* namespace impl */

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

impl::BusDeviceCreator BusDeviceCreator;
