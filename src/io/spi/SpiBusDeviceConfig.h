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

#ifndef SPI_BUS_DEVICE_CONFIG_H_
#define SPI_BUS_DEVICE_CONFIG_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino.h>

#include <SPI.h>

#include <functional>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class SpiBusDeviceConfig
{
public:

  typedef std::function<void(void)> SpiSelectFunc;
  typedef std::function<void(void)> SpiDeselectFunc;


  SpiBusDeviceConfig(arduino::HardwareSPI & spi, SPISettings const & spi_settings, SpiSelectFunc spi_select, SpiDeselectFunc spi_deselect, byte const fill_symbol = 0xFF)
  : _spi{spi}
  , _spi_settings{spi_settings}
  , _spi_select{spi_select}
  , _spi_deselect{spi_deselect}
  , _fill_symbol{fill_symbol}
  { }

  SpiBusDeviceConfig(arduino::HardwareSPI & spi, SPISettings const & spi_settings, int const cs_pin, byte const fill_symbol = 0xFF)
  : SpiBusDeviceConfig
    {spi,
     spi_settings,
     [cs_pin](){ digitalWrite(cs_pin, LOW); },
     [cs_pin](){ digitalWrite(cs_pin, HIGH); },
     fill_symbol
    }
  { }


  arduino::HardwareSPI & spi() { return _spi; }
  SPISettings settings   () const { return _spi_settings; }
  void        select     () const { if (_spi_select) _spi_select(); }
  void        deselect   () const { if (_spi_deselect) _spi_deselect(); }
  byte        fillSymbol () const { return _fill_symbol; }

  SpiSelectFunc   selectFunc  () const { return _spi_select;  }
  SpiDeselectFunc deselectFunc() const { return _spi_deselect;  }

private:

  arduino::HardwareSPI & _spi;
  SPISettings _spi_settings;
  SpiSelectFunc _spi_select{nullptr};
  SpiDeselectFunc _spi_deselect{nullptr};
  byte _fill_symbol{0xFF};

};

#endif /* SPI_BUS_DEVICE_CONFIG_H_ */
