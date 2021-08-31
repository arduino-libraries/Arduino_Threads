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

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "SerialDispatcher.h"

/**************************************************************************************
 * CTOR/DTOR
 **************************************************************************************/

SerialDispatcher::SerialDispatcher(arduino::HardwareSerial & serial)
: _is_initialized{false}
, _mutex{}
, _cond{_mutex}
, _serial{serial}
, _thread(osPriorityRealtime, 4096, nullptr, "SerialDispatcher")
, _has_tread_started{false}
, _terminate_thread{false}
{

}

/**************************************************************************************
 * PUBLIC MEMBER FUNCTIONS
 **************************************************************************************/

void SerialDispatcher::begin(unsigned long baudrate)
{
  begin(baudrate, SERIAL_8N1);
}

void SerialDispatcher::begin(unsigned long baudrate, uint16_t config)
{
  if (!_is_initialized)
  {
    _serial.begin(baudrate, config);
    _is_initialized = true;
    _thread.start(mbed::callback(this, &SerialDispatcher::threadFunc)); /* TODO: Check return code */
    while (!_has_tread_started) { }
  }

  mbed::ScopedLock<rtos::Mutex> lock(_mutex);

  /* Check if the thread calling begin is already in the list. */
  osThreadId_t const current_thread_id = rtos::ThisThread::get_id();
  if (findThreadCustomerDataById(rtos::ThisThread::get_id()) == std::end(_thread_customer_list))
  {
    /* Since the thread is not in the list yet we are
     * going to create a new entry to the list.
     */
    ThreadCustomerData data;
    data.thread_id = current_thread_id;
    _thread_customer_list.push_back(data);
  }
}

void SerialDispatcher::end()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);

  /* Retrieve the current thread id and remove
   * the thread data from the thread data list.
   */
  osThreadId_t const current_thread_id = rtos::ThisThread::get_id();
  std::remove_if(std::begin(_thread_customer_list),
                 std::end  (_thread_customer_list),
                 [current_thread_id](ThreadCustomerData const d) -> bool { return (d.thread_id == current_thread_id); });

  /* If no thread consumers are left also end
   * the serial device alltogether.
   */
  if (_thread_customer_list.size() == 0)
  {
    _terminate_thread = true;
    _thread.join();
    _serial.end();
  }
}

int SerialDispatcher::available()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  return _serial.available();
}

int SerialDispatcher::peek()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  return _serial.peek();
}

int SerialDispatcher::read()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  return _serial.read();
}

void SerialDispatcher::flush()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  _serial.flush();
}

size_t SerialDispatcher::write(uint8_t const b)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);

  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());

  /* If this thread hasn't registered yet
   * with the SerialDispatcher via 'begin'.
   */
  if (iter == std::end(_thread_customer_list))
    return 0;

  if (iter->tx_buffer.availableForStore())
    iter->tx_buffer.store_char(b);

  /* Inform the worker thread that new data has
   * been written to a Serial transmit buffer.
   */
  _cond.notify_one();

  return 1;
}

size_t SerialDispatcher::write(const uint8_t * data, size_t len)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);

  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());

  /* If this thread hasn't registered yet
   * with the SerialDispatcher via 'begin'.
   */
  if (iter == std::end(_thread_customer_list))
    return 0;

  size_t bytes_written = 0;
  for (; (bytes_written < len) && iter->tx_buffer.availableForStore(); bytes_written++)
    iter->tx_buffer.store_char(data[bytes_written]);

  /* Inform the worker thread that new data has
   * been written to a Serial transmit buffer.
   */
  _cond.notify_one();

  return bytes_written;
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

void SerialDispatcher::threadFunc()
{
  _has_tread_started = true;

  while(!_terminate_thread)
  {
      /* Prevent race conditions by multi-threaded
       * access to shared data.
       */
      mbed::ScopedLock<rtos::Mutex> lock(_mutex);
      /* Wait for new data to be available */      
      _cond.wait();
      /* Iterate over all list entries. */
      std::for_each(std::begin(_thread_customer_list),
                    std::end  (_thread_customer_list),
                    [this](ThreadCustomerData & d)
                    {
                      while(d.tx_buffer.available())
                      {
                        _serial.write(d.tx_buffer.read_char());
                      }
                    });
  }
}

std::list<SerialDispatcher::ThreadCustomerData>::iterator SerialDispatcher::findThreadCustomerDataById(osThreadId_t const thread_id)
{
  return std::find_if(std::begin(_thread_customer_list), 
                      std::end  (_thread_customer_list),
                      [thread_id](ThreadCustomerData const d) -> bool { return (d.thread_id == thread_id); });
}