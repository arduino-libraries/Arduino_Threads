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

#ifndef BUS_DEVICE_H_
#define BUS_DEVICE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "IoTransaction.h"

#include "spi/SpiBusDeviceConfig.h"

/**************************************************************************************
 * FORWARD DECLARATION
 **************************************************************************************/

namespace arduino
{
  class HardwareSPI;
  class HardwareI2C;
}

class BusDevice;
class SpiBusDevice;
class WireBusDevice;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class BusDeviceBase
{
public:

  virtual ~BusDeviceBase() { }

  virtual IoResponse transfer(IoRequest & req) = 0;


  static BusDevice create(arduino::HardwareSPI & spi, int const cs_pin, SPISettings const & spi_settings, byte const fill_symbol = 0xFF);
  static BusDevice create(arduino::HardwareSPI & spi, int const cs_pin, uint32_t const spi_clock, BitOrder const spi_bit_order, SPIMode const spi_bit_mode, byte const fill_symbol = 0xFF);
  static BusDevice create(arduino::HardwareSPI & spi, SpiBusDeviceConfig::SpiSelectFunc spi_select, SpiBusDeviceConfig::SpiDeselectFunc spi_deselect, SPISettings const & spi_settings, byte const fill_symbol = 0xFF);

  static BusDevice create(arduino::HardwareI2C & wire, byte const slave_addr);
  static BusDevice create(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart);
  static BusDevice create(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart, bool const stop);

};

class BusDevice
{
public:

  BusDevice(BusDeviceBase * dev);

  BusDevice(arduino::HardwareSPI & spi, int const cs_pin, SPISettings const & spi_settings, byte const fill_symbol = 0xFF);
  BusDevice(arduino::HardwareSPI & spi, int const cs_pin, uint32_t const spi_clock, BitOrder const spi_bit_order, SPIMode const spi_bit_mode, byte const fill_symbol = 0xFF);
  BusDevice(arduino::HardwareSPI & spi, SpiBusDeviceConfig::SpiSelectFunc spi_select, SpiBusDeviceConfig::SpiDeselectFunc spi_deselect, SPISettings const & spi_settings, byte const fill_symbol = 0xFF);

  BusDevice(arduino::HardwareI2C & wire, byte const slave_addr);
  BusDevice(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart);
  BusDevice(arduino::HardwareI2C & wire, byte const slave_addr, bool const restart, bool const stop);

  IoResponse transfer(IoRequest & req);


  SpiBusDevice  & spi();
  WireBusDevice & wire();


private:

  mbed::SharedPtr<BusDeviceBase> _dev;

};

#endif /* BUS_DEVICE_H_ */
