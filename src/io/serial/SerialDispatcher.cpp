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
, _serial{serial}
, _thread(osPriorityRealtime, 4096, nullptr, "SerialDispatcher")
, _has_tread_started{false}
, _terminate_thread{false}
, _global_prefix_callback{nullptr}
, _global_suffix_callback{nullptr}
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
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);

  if (!_is_initialized)
  {
    _serial.begin(baudrate, config);
    _is_initialized = true;
    _thread.start(mbed::callback(this, &SerialDispatcher::threadFunc)); /* TODO: Check return code */
    while (!_has_tread_started) { }
  }

  /* Check if the thread calling begin is already in the list. */
  osThreadId_t const current_thread_id = rtos::ThisThread::get_id();
  if (findThreadCustomerDataById(rtos::ThisThread::get_id()) == std::end(_thread_customer_list))
  {
    /* Since the thread is not in the list yet we are
     * going to create a new entry to the list.
     */
    uint32_t const thread_event_flag = (1<<(_thread_customer_list.size()));
    ThreadCustomerData data{current_thread_id, thread_event_flag};
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
                 [current_thread_id](ThreadCustomerData const d) -> bool
                 {
                   return (d.thread_id == current_thread_id);
                 });

  /* If no thread consumers are left also end
   * the serial device altogether.
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
  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());
  assert(iter != std::end(_thread_customer_list));

  prepareSerialReader(iter);
  handleSerialReader();

  return iter->rx_buffer->available();
}

int SerialDispatcher::peek()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());
  assert(iter != std::end(_thread_customer_list));

  prepareSerialReader(iter);
  handleSerialReader();

  return iter->rx_buffer->peek();
}

int SerialDispatcher::read()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());
  assert(iter != std::end(_thread_customer_list));

  prepareSerialReader(iter);
  handleSerialReader();

  return iter->rx_buffer->read_char();
}

void SerialDispatcher::flush()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  _serial.flush();
}

size_t SerialDispatcher::write(uint8_t const b)
{
  return write(&b, 1);
}

size_t SerialDispatcher::write(const uint8_t * data, size_t len)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());
  assert(iter != std::end(_thread_customer_list));

  size_t bytes_written = 0;
  for (; (bytes_written < len) && iter->tx_buffer.availableForStore(); bytes_written++)
    iter->tx_buffer.store_char(data[bytes_written]);

  /* Inform the worker thread that new data has
   * been written to a Serial transmit buffer.
   */
  _data_available_for_transmit.set(iter->thread_event_flag);

  return bytes_written;
}

void SerialDispatcher::block()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());
  assert(iter != std::end(_thread_customer_list));

  iter->block_tx_buffer = true;
}

void SerialDispatcher::unblock()
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());
  assert(iter != std::end(_thread_customer_list));

  iter->block_tx_buffer = false;

  _data_available_for_transmit.set(iter->thread_event_flag);
}

void SerialDispatcher::prefix(PrefixInjectorCallbackFunc func)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());
  assert(iter != std::end(_thread_customer_list));

  iter->prefix_func = func;
}

void SerialDispatcher::suffix(SuffixInjectorCallbackFunc func)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  auto iter = findThreadCustomerDataById(rtos::ThisThread::get_id());
  assert(iter != std::end(_thread_customer_list));

  iter->suffix_func = func;
}

void SerialDispatcher::globalPrefix(PrefixInjectorCallbackFunc func)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  _global_prefix_callback = func;
}

void SerialDispatcher::globalSuffix(SuffixInjectorCallbackFunc func)
{
  mbed::ScopedLock<rtos::Mutex> lock(_mutex);
  _global_suffix_callback = func;
}

/**************************************************************************************
 * PRIVATE MEMBER FUNCTIONS
 **************************************************************************************/

void SerialDispatcher::threadFunc()
{
  _has_tread_started = true;

  while(!_terminate_thread)
  {
      /* Wait for data to be available in a transmit buffer. */
      static uint32_t constexpr ALL_EVENT_FLAGS = 0x7fffffff;
      _data_available_for_transmit.wait_any(ALL_EVENT_FLAGS, osWaitForever, /* clear */ true);

      /* Iterate over all list entries. */
      std::for_each(std::begin(_thread_customer_list),
                    std::end  (_thread_customer_list),
                    [this](ThreadCustomerData & d)
                    {
                      if (d.block_tx_buffer)
                        return;

                      /* Return if there's no data to be written to the
                       * serial interface. This statement is necessary
                       * because otherwise the prefix/suffix functions
                       * will be invoked and will be printing something,
                       * even though no data is actually to be printed for
                       * most threads.
                       */
                      if (!d.tx_buffer.available())
                        return;

                      /* Retrieve all data stored in the transmit ringbuffer
                       * and store it into a String for usage by both suffix
                       * prefix callback functions.
                       */
                      String msg;
                      while(d.tx_buffer.available())
                        msg += static_cast<char>(d.tx_buffer.read_char());

                      /* The prefix callback function allows the
                       * user to insert a custom message before
                       * a new message is written to the serial
                       * driver. This is useful e.g. for wrapping
                       * protocol (e.g. the 'AT' protocol) or providing
                       * a timestamp, a log level, ...
                       */
                      String prefix;
                      if (d.prefix_func)
                        prefix = d.prefix_func(msg);
                      /* A prefix callback function defined per thread
                       * takes precedence over a globally defined prefix
                       * callback function.
                       */
                      else if (_global_prefix_callback)
                        prefix = _global_prefix_callback(msg);

                      /* Similar to the prefix function this callback
                       * allows the user to specify a specific message
                       * to be appended to each message, e.g. '\r\n'.
                       */
                      String suffix;
                      if (d.suffix_func)
                        suffix = d.suffix_func(prefix, msg);
                      /* A suffix callback function defined per thread
                       * takes precedence over a globally defined suffix
                       * callback function.
                       */
                      else if (_global_suffix_callback)
                        suffix = _global_suffix_callback(prefix, msg);

                      /* Now it's time to actually write the message
                       * conveyed by the user via Serial.print/println.
                       */
                      _serial.write(prefix.c_str());
                      _serial.write(msg.c_str());
                      _serial.write(suffix.c_str());
                    });
  }
}

std::list<SerialDispatcher::ThreadCustomerData>::iterator SerialDispatcher::findThreadCustomerDataById(osThreadId_t const thread_id)
{
  return std::find_if(std::begin(_thread_customer_list), 
                      std::end  (_thread_customer_list),
                      [thread_id](ThreadCustomerData const d) -> bool
                      {
                        return (d.thread_id == thread_id);
                      });
}

void SerialDispatcher::prepareSerialReader(std::list<ThreadCustomerData>::iterator & iter)
{
  if (!iter->rx_buffer)
    iter->rx_buffer.reset(new arduino::RingBuffer());
}

void SerialDispatcher::handleSerialReader()
{
  while (_serial.available())
  {
    int const c = _serial.read();

    std::for_each(std::begin(_thread_customer_list),
                  std::end  (_thread_customer_list),
                  [c](ThreadCustomerData & d)
                  {
                    if (!d.rx_buffer)
                      return;

                    if (!d.rx_buffer->availableForStore())
                      return;

                    d.rx_buffer->store_char(c);
                  });
  }
}

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino.h>

/**************************************************************************************
 * GLOBAL VARIABLE DECLARATION
 **************************************************************************************/

#ifdef ARDUINO_PORTENTA_H7_M4
  SerialDispatcher Serial(Serial1); /* No SerialUSB for Portenta H7 / M4 Core */
#else
  SerialDispatcher Serial(SerialUSB);
#endif
