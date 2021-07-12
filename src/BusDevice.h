/*
 * This file is part of the Arduino_ThreadsafeIO library.
 * Copyright (c) 2021 Arduino SA.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef BUS_DEVICE_H_
#define BUS_DEVICE_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "IoTransaction.h"

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class BusDevice
{
public:
  virtual ~BusDevice() { }

  virtual IoResponse transfer(IoRequest & req) = 0;
};

#endif /* BUS_DEVICE_H_ */
