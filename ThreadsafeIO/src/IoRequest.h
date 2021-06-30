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

  IoRequest(Type const type, uint8_t const * const tx_buf, size_t const tx_buf_len, uint8_t * rx_buf, size_t * rx_buf_len)
  : _type{type}
  , _tx_buf{tx_buf}
  , _tx_buf_len{tx_buf_len}
  , _rx_buf{rx_buf}
  , _rx_buf_len{rx_buf_len}
  { }

/*
  inline Type            type      () const { return _type; }
  inline uint8_t const * tx_buf    () const { return _tx_buf; }
  inline size_t          tx_buf_len() const { return _tx_buf_len; }
  inline uint8_t *       rx_buf    ()       { return _rx_buf; }
  inline size_t *        rx_buf_len()       { return _rx_buf_len; }

private:
*/
  Type _type{Type::None};
  uint8_t const * const _tx_buf{nullptr};
  size_t const _tx_buf_len{0};
  uint8_t * _rx_buf{nullptr};
  size_t * _rx_buf_len{0};
};

#endif /* IO_REQUEST_H_ */