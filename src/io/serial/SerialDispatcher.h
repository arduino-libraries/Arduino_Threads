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

#ifndef SERIAL_DISPATCHER_H_
#define SERIAL_DISPATCHER_H_

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include "api/HardwareSerial.h"

#include <mbed.h>

#include <list>
#include <functional>

#include <SharedPtr.h>

/**************************************************************************************
 * CLASS DECLARATION
 **************************************************************************************/

class SerialDispatcher : public arduino::HardwareSerial
{

public:

  SerialDispatcher(arduino::HardwareSerial & serial);


  virtual void begin(unsigned long baudrate) override;
  virtual void begin(unsigned long baudrate, uint16_t config) override;
  virtual void end() override;
  virtual int available() override;
  virtual int peek() override;
  virtual int read() override;
  virtual void flush() override;
  virtual size_t write(uint8_t const b) override;
  virtual size_t write(const uint8_t * data, size_t len) override;
  using Print::write;
  virtual operator bool() override { return _serial; }

  void block();
  void unblock();

  typedef std::function<String(String const &)> PrefixInjectorCallbackFunc;
  typedef std::function<String(String const &, String const &)>  SuffixInjectorCallbackFunc;
  void prefix(PrefixInjectorCallbackFunc func);
  void suffix(SuffixInjectorCallbackFunc func);
  void globalPrefix(PrefixInjectorCallbackFunc func);
  void globalSuffix(SuffixInjectorCallbackFunc func);


private:

  bool _is_initialized;
  rtos::Mutex _mutex;
  rtos::EventFlags _data_available_for_transmit;
  arduino::HardwareSerial & _serial;

  rtos::Thread _thread;
  bool _has_tread_started;
  bool _terminate_thread;

  PrefixInjectorCallbackFunc _global_prefix_callback;
  SuffixInjectorCallbackFunc _global_suffix_callback;

  static int constexpr THREADSAFE_SERIAL_TRANSMIT_RINGBUFFER_SIZE = 128;
  typedef arduino::RingBufferN<THREADSAFE_SERIAL_TRANSMIT_RINGBUFFER_SIZE> SerialTransmitRingbuffer;

  class ThreadCustomerData
  {
  public:
    ThreadCustomerData(osThreadId_t const t, uint32_t const t_event_flag)
    : thread_id{t}
    , thread_event_flag{t_event_flag}
    , tx_buffer{}
    , block_tx_buffer{false}
    , rx_buffer{}
    , prefix_func{nullptr}
    , suffix_func{nullptr}
    { }

    osThreadId_t thread_id;
    uint32_t thread_event_flag;
    SerialTransmitRingbuffer tx_buffer;
    bool block_tx_buffer;
    mbed::SharedPtr<arduino::RingBuffer> rx_buffer; /* Only when a thread has expressed interested to read from serial a receive ringbuffer is allocated. */
    PrefixInjectorCallbackFunc prefix_func;
    SuffixInjectorCallbackFunc suffix_func;
  };

  std::list<ThreadCustomerData> _thread_customer_list;

  void threadFunc();
  std::list<ThreadCustomerData>::iterator findThreadCustomerDataById(osThreadId_t const thread_id);
  void prepareSerialReader(std::list<ThreadCustomerData>::iterator & iter);
  void handleSerialReader();
};

/**************************************************************************************
 * EXTERN DECLARATION
 **************************************************************************************/

#undef Serial
extern SerialDispatcher Serial;

#endif /* SERIAL_DISPATCHER_H_ */
