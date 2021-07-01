/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef SPI_BUS_DEVICE_CONFIG_H_
#define SPI_BUS_DEVICE_CONFIG_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <functional>
#include <Arduino.h>
#include <SPI.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class SpiBusDeviceConfig
{
public:
  typedef std::function<void(void)> SpiSelectFunc;
  typedef std::function<void(void)> SpiDeselectFunc;
  SpiBusDeviceConfig(SPISettings const & spi_settings, SpiSelectFunc spi_select, SpiDeselectFunc spi_deselect, uint8_t const fill_symbol = 0xFF)
  : _spi_settings{spi_settings}
  , _spi_select{spi_select}
  , _spi_deselect{spi_deselect}
  , _fill_symbol{fill_symbol}
  { }
  SPISettings settings   () const { return _spi_settings; }
  void        select     () const { if (_spi_select) _spi_select(); }
  void        deselect   () const { if (_spi_deselect) _spi_deselect(); }
  uint8_t     fill_symbol() const { return _fill_symbol; }
private:
  SPISettings _spi_settings;
  SpiSelectFunc _spi_select{nullptr};
  SpiDeselectFunc _spi_deselect{nullptr};
  uint8_t _fill_symbol{0xFF};
};

#endif /* SPI_BUS_DEVICE_CONFIG_H_ */
