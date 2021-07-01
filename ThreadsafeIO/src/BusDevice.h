/*
 * A deeply magical library providing threadsafe IO via pipes.
 */

#ifndef BUS_DEVICE_H_
#define BUS_DEVICE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "IoRequest.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class BusDevice
{
public:
  virtual ~BusDevice() { }

  enum class Status : int
  {
    Ok = 0,
  };

  virtual Status transfer(IoRequest & req) = 0;
};

#endif /* BUS_DEVICE_H_ */
