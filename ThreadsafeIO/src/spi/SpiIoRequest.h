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

  SpiIoRequest(uint8_t const * const write_buf, size_t const bytes_to_write, uint8_t * read_buf, size_t const bytes_to_read)
  : IoRequest(IoRequest::Type::SPI, write_buf, bytes_to_write, read_buf, bytes_to_read)
  { }

  void set_config(SpiBusDeviceConfig * config) { _config = config; }
  SpiBusDeviceConfig & config() { return *_config; }

private:

  SpiBusDeviceConfig * _config{nullptr};
};

#endif /* SPI_IO_REQUEST_H_ */