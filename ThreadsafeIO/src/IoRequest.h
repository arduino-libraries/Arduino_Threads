/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef IO_REQUEST_H_
#define IO_REQUEST_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <stdint.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class IoRequest
{
public:

  enum class Type
  {
    None,
    SPI
  };

  class WriteBuf
  {
  public:
    WriteBuf(uint8_t const * const d, size_t const b2w) : data{d}, bytes_to_write{b2w} { }
    uint8_t const * const data{nullptr};
    size_t const bytes_to_write{0};
  };

  class ReadBuf
  {
  public:
    ReadBuf(uint8_t * d, size_t const b2r, size_t * br) : data{d}, bytes_to_read{b2r}, bytes_read{br} { }
    uint8_t * data{nullptr};
    size_t const bytes_to_read{0};
    size_t * bytes_read{0};
  };


  IoRequest(Type const type, uint8_t const * const tx_buf, size_t const tx_buf_len, uint8_t * rx_buf, size_t const rx_buf_len, size_t * bytes_read)
  : _type{type}
  , _write_buf{tx_buf, tx_buf_len}
  , _read_buf{rx_buf, rx_buf_len, bytes_read}
  { }


  inline Type       type     () const { return _type; }
  inline WriteBuf & write_buf()       { return _write_buf; }
  inline ReadBuf  & read_buf ()       { return _read_buf; }

private:

  Type _type{Type::None};
  WriteBuf _write_buf;
  ReadBuf _read_buf;
};

#include "SpiBusDeviceConfig.h"

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

#endif /* IO_REQUEST_H_ */