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

#include "BusDevice.h"

#include "spi/SpiBusDevice.h"
#include "wire/WireBusDevice.h"

/**************************************************************************************
 * BusDeviceBase PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

BusDevice BusDeviceBase::create(arduino::HardwareSPI & spi, int const cs_pin, SPISettings const & spi_settings, byte const fill_symbol)
{
  return BusDevice(new SpiBusDevice(SpiBusDeviceConfig{spi,
                                                       spi_settings,
                                                       cs_pin,
                                                       fill_symbol
                                                      }));
}

BusDevice BusDeviceBase::create(arduino::HardwareSPI & spi, int const cs_pin, uint32_t const spi_clock, BitOrder const spi_bit_order, SPIMode const spi_bit_mode, byte const fill_symbol)
{
  return BusDevice(new SpiBusDevice(SpiBusDeviceConfig{spi,
                                                       SPISettings(spi_clock, spi_bit_order, spi_bit_mode),
                                                       cs_pin,
                                                       fill_symbol
                                                      }));
}

BusDevice BusDeviceBase::create(arduino::HardwareSPI & spi, SpiBusDeviceConfig::SpiSelectFunc spi_select, SpiBusDeviceConfig::SpiDeselectFunc spi_deselect, SPISettings const & spi_settings, byte const fill_symbol)
{
  return BusDevice(new SpiBusDevice(SpiBusDeviceConfig{spi, spi_settings, spi_select, spi_deselect, fill_symbol}));
}

BusDevice BusDeviceBase::create(arduino::HardwareI2C & wire, byte const slave_addr)
{
  return create(wire, slave_addr, true, true);
}

BusDevice BusDeviceBase::create(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart)
{
  return create(wire, slave_addr, restart, true);
}

BusDevice BusDeviceBase::create(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart, bool const stop)
{
  return BusDevice(new WireBusDevice(WireBusDeviceConfig{wire, slave_addr, restart, stop}));
}

/**************************************************************************************
 * BusDevice CTOR/DTOR
 **************************************************************************************/

BusDevice::BusDevice(BusDeviceBase * dev)
: _dev{dev}
{ }

BusDevice::BusDevice(arduino::HardwareSPI & spi, int const cs_pin, SPISettings const & spi_settings, byte const fill_symbol)
{
  *this = BusDeviceBase::create(spi, cs_pin, spi_settings, fill_symbol);
}

BusDevice::BusDevice(arduino::HardwareSPI & spi, int const cs_pin, uint32_t const spi_clock, BitOrder const spi_bit_order, SPIMode const spi_bit_mode, byte const fill_symbol)
{
  *this = BusDeviceBase::create(spi, cs_pin, spi_clock, spi_bit_order, spi_bit_mode, fill_symbol);
}

BusDevice::BusDevice(arduino::HardwareSPI & spi, SpiBusDeviceConfig::SpiSelectFunc spi_select, SpiBusDeviceConfig::SpiDeselectFunc spi_deselect, SPISettings const & spi_settings, byte const fill_symbol)
{
  *this = BusDeviceBase::create(spi, spi_select, spi_deselect, spi_settings, fill_symbol);
}

BusDevice::BusDevice(arduino::HardwareI2C & wire, byte const slave_addr)
{
  *this = BusDeviceBase::create(wire, slave_addr);
}

BusDevice::BusDevice(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart)
{
  *this = BusDeviceBase::create(wire, slave_addr, restart);
}

BusDevice::BusDevice(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart, bool const stop)
{
  *this = BusDeviceBase::create(wire, slave_addr, restart, stop);
}

IoResponse BusDevice::transfer(IoRequest & req)
{
  return _dev->transfer(req);
}

SpiBusDevice & BusDevice::spi()
{
  return *reinterpret_cast<SpiBusDevice *>(_dev.get());
}

WireBusDevice & BusDevice::wire()
{
  return *reinterpret_cast<WireBusDevice *>(_dev.get());
}
