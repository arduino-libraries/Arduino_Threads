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

#ifndef ARDUINO_THREADS_SOURCE_HPP_
#define ARDUINO_THREADS_SOURCE_HPP_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <list>
#include <algorithm>

/**************************************************************************************
 * FORWARD DECLARATION
 **************************************************************************************/

template<class T>
class SinkBase;

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

template<typename T>
class Source
{
public:

  void connectTo(SinkBase<T> & sink);
  void push(T const & val);

private:
  std::list<SinkBase<T> *> _sink_list;
};

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

template<typename T>
void Source<T>::connectTo(SinkBase<T> & sink)
{
  _sink_list.push_back(&sink);
}

template<typename T>
void Source<T>::push(T const & val)
{
  std::for_each(std::begin(_sink_list),
                std::end  (_sink_list),
                [val](SinkBase<T> * sink)
                {
                  sink->inject(val);
                });
}

#endif /* ARDUINO_THREADS_SOURCE_HPP_ */
