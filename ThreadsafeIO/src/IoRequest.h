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

  IoRequest(Type const type, uint8_t const * const tx_buf, size_t const tx_buf_len, uint8_t * rx_buf, size_t const rx_buf_len, size_t * bytes_read)
  : _type{type}
  , _write_buf{tx_buf, tx_buf_len}
  , _read_buf{rx_buf, rx_buf_len, bytes_read}
  { }

/*
  inline Type            type      () const { return _type; }
  inline uint8_t const * tx_buf    () const { return _write_buf; }
  inline size_t          tx_buf_len() const { return _write_buf_len; }
  inline uint8_t *       rx_buf    ()       { return _read_buf; }
  inline size_t *        rx_buf_len()       { return _read_buf_len; }

private:
*/
  Type _type{Type::None};

  class WriteBuf
  {
  public:
    WriteBuf(uint8_t const * const b, size_t const l) : buf{b}, len{l} { }
    uint8_t const * const buf{nullptr};
    size_t const len{0};
  };

  class ReadBuf
  {
  public:
    ReadBuf(uint8_t * b, size_t const l, size_t * br) : buf{b}, len{l}, bytes_read{br} { }
    uint8_t * buf{nullptr};
    size_t const len{0};
    size_t * bytes_read{0};
  };

  WriteBuf _write_buf;
  ReadBuf _read_buf;
};

#endif /* IO_REQUEST_H_ */