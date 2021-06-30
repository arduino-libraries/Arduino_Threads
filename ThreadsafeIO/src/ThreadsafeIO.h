/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef THREADSAFE_IO_H_
#define THREADSAFE_IO_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <algorithm>
#include <functional>

#include <Arduino.h>
#include <SPI.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

/**************************************************************************************
 * IoRequest
 **************************************************************************************/

class IoRequest
{
public:
  enum class Type { None, SPI };

  IoRequest(Type const type, uint8_t const * const tx_buf, size_t const tx_buf_len, uint8_t * rx_buf, size_t * rx_buf_len)
  : _type{type}
  , _tx_buf{tx_buf}
  , _tx_buf_len{tx_buf_len}
  , _rx_buf{rx_buf}
  , _rx_buf_len{rx_buf_len}
  { }
  
  Type _type{Type::None};
  uint8_t const * const _tx_buf{nullptr};
  size_t const _tx_buf_len{0};
  uint8_t * _rx_buf{nullptr};
  size_t * _rx_buf_len{0};
};

class SpiIoRequest : public IoRequest
{
public:
  SpiIoRequest(uint8_t const * const tx_buf, size_t const tx_buf_len, uint8_t * rx_buf, size_t * rx_buf_len)
  : IoRequest(IoRequest::Type::SPI, tx_buf, tx_buf_len, rx_buf, rx_buf_len)
  { }
};

/**************************************************************************************
 * BusDevice
 **************************************************************************************/

class BusDevice
{
public:
  virtual ~BusDevice() { }
  enum class Status : int
  {
    Ok = 0,
    ConfigError = -1,
  };
  virtual Status transfer(IoRequest const & req) = 0;
};

/**************************************************************************************
 * SpiBusDevice
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

class SpiBusDevice : public BusDevice
{
public:
  SpiBusDevice(std::string const & spi_bus, SpiBusDeviceConfig const & config) : _config{config}
  {
    /* TODO: Select SPI bus based in string. */
  }
  virtual Status transfer(IoRequest const & req) override
  {
    /* TODO: Instead of directly processing in here push
     * the whole request into a queue and hand over processing
     * to the IO thread.
     */
    if (!_config.good())
      return Status::ConfigError;

    _config.select();

    SPI.beginTransaction(_config.settings());

    size_t bytes_received = 0,
           bytes_sent = 0;
    for(; bytes_received < (*req._rx_buf_len); bytes_received++, bytes_sent++)
    {
      uint8_t tx_byte = 0;

      if (bytes_sent < req._tx_buf_len)
        tx_byte = req._tx_buf[bytes_sent];
      else
        tx_byte = _config.fill_symbol();

      req._rx_buf[bytes_received] = SPI.transfer(tx_byte);
    }
    *req._rx_buf_len = bytes_received;

    SPI.endTransaction();

    _config.deselect();

    return Status::Ok;
  }
private:
  SpiBusDeviceConfig _config;
};

/**************************************************************************************
 * Stuff
 **************************************************************************************/

typedef int io_error_t;

class SpiIoManager
{
public:

  io_error_t request(/* IoTransactionRequest const & req */);


private:

  void threadFunc();

};

#endif /* THREADSAFE_IO_H_ */
