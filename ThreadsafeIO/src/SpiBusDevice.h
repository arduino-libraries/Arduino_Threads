/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef SPI_BUS_DEVICE_H_
#define SPI_BUS_DEVICE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <functional>

#include <Arduino.h>
#include <mbed.h>
#include <SPI.h>

#include "IoRequest.h"
#include "BusDevice.h"

#include "SpiDispatcher.h"

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
  bool        good       () const { return (_spi_select && _spi_deselect); }
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

class SpiIoRequest : public IoRequest
{
public:
  SpiIoRequest(uint8_t const * const tx_buf, size_t const tx_buf_len, uint8_t * rx_buf, size_t const rx_buf_len, size_t * bytes_read)
  : IoRequest(IoRequest::Type::SPI, tx_buf, tx_buf_len, rx_buf, rx_buf_len, bytes_read)
  { }

  void set_config(SpiBusDeviceConfig * config) { _config = config; }
  SpiBusDeviceConfig & config() { return *_config; }

private:
  SpiBusDeviceConfig * _config{nullptr};
};

class SpiBusDevice : public BusDevice
{
public:
  SpiBusDevice(std::string const & spi_bus, SpiBusDeviceConfig const & config) : _config{config}
  {
    /* TODO: Select SPI bus based in string. */
  }
  virtual Status transfer(IoRequest & req) override
  {
    if (!_config.good())
      return Status::ConfigError;

    reinterpret_cast<SpiIoRequest*>(&req)->set_config(&_config);

    /* Insert into queue. */
    SpiDispatcher::instance().request(&req);

    return Status::Ok;
  }

private:

  SpiBusDeviceConfig _config;

};

#endif /* SPI_BUS_DEVICE_H_ */