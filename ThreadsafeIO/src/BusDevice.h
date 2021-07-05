/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef BUS_DEVICE_H_
#define BUS_DEVICE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "IoRequest.h"
#include "IoResponse.h"

#include <SharedPtr.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class BusDevice
{
public:
  virtual ~BusDevice() { }

  virtual TSharedIoResponse transfer(IoRequest & req) = 0;
};

#endif /* BUS_DEVICE_H_ */
