/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef SPI_IO_REQUEST_H_
#define SPI_IO_REQUEST_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "../IoRequest.h"

#include "SpiBusDeviceConfig.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

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

#endif /* SPI_IO_REQUEST_H_ */