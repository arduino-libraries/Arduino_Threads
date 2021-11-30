<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

Threadsafe `Wire`
=================
## Introduction
A key problem of multi-tasking is the prevention of erroneous state when multiple threads share a single resource. The following example borrowed from a typical application demonstrates the problems resulting from multiple threads sharing a single resource:

Imagine a embedded system where multiple `Wire` client devices are physically connected to a single `Wire` server. Each `Wire` client device is managed by a dedicated software thread. Each thread polls its `Wire` client device periodically. Access to the `Wire` bus is managed via the `Wire` library and typically follows the pattern described below:
```C++
/* Wire Write Access */
Wire.beginTransmission(addr);
Wire.write(val);
Wire.endTransmission();

/* Wire Read Access */
Wire.beginTransmission(addr);
Wire.write(val);
Wire.endTransmission();
Wire.requestFrom(addr, bytes)
while(Wire.available()) {
  int val = Wire.read();
}
```
Since we are using a [preemptive](https://os.mbed.com/docs/mbed-os/v6.11/program-setup/concepts.html#threads) [RTOS](https://en.wikipedia.org/wiki/Real-time_operating_system) [ARM Mbed OS](https://os.mbed.com/mbed-os/) with a tick time of 10 ms for achieving multi-tasking capability and under the assumption that all threads share the same priority (which leads to a [round-robin scheduling](https://en.wikipedia.org/wiki/Round-robin_scheduling)) it can easily happen that one thread is half-way through its `Wire` access when the scheduler interrupts it and schedules the next thread which in turn starts/continues/ends its own `Wire` access.

As a result this interruption by the scheduler will break `Wire` access for both devices and leave the `Wire` controller in an undefined state. 

In Arduino Parallela we introduced the concept of `BusDevice`s which are meant to unify the way sketches access peripherals through heterogeneous busses such as `Wire`, `SPI` and `Serial`. A `BusDevice` is declared simply by specifying the type of interface and its parameters: 
```C++
BusDevice lsm6dsox(Wire, LSM6DSOX_ADDRESS);
/* or */
BusDevice lsm6dsox(Wire, LSM6DSOX_ADDRESS, true /* restart */);
/* or */
BusDevice lsm6dsox(Wire, LSM6DSOX_ADDRESS, false /* restart */, true, /* stop */);
```

### `transfer`/`wait` **asynchronous** threadsafe `Wire` access
Once a `BusDevice` is declared it can be used to transfer data to and from the peripheral by means of the `transfer()` API. As opposed to traditional Arduino bus APIs `transfer()` is asynchronous and thus won't block execution unless the `wait()` function is called.
Note that since we are in a parallel programming environment this means that calls to `transfer()` on the same bus from different sketches will be arbitrated and that the `wait()` API will suspend the sketch until the transfer is complete, thus allowing other processes to execute or going to low power state.
```C++
byte lsm6dsox_read_reg(byte const reg_addr)
{
  byte write_buf = reg_addr;
  byte read_buf  = 0;
  
  IoRequest  req(write_buf, read_buf);
  IoResponse rsp = lsm6dsox.transfer(req);
  /* Do other stuff */
  rsp->wait(); /* Wait for the completion of the IO Request. */
 
  return read_buf;
}
```

### `transfer_and_wait` **synchronous** threadsafe `Wire` access
([`examples/Threadsafe_IO/Wire`](../examples/Threadsafe_IO/Wire))

As the use of the `transfer` API might be confusing there's also a synchronous API call combining the request of the transfer and waiting for it's result using `transfer_and_wait`.
```C++
byte lsm6dsox_read_reg(byte const reg_addr)
{
  byte write_buf = reg_addr;
  byte read_buf  = 0;
  
  IoRequest  req(write_buf, read_buf);
  IoResponse rsp = transfer_and_wait(lsm6dsox, req); /* Transmit IO request for execution and wait for completion of request. */
  
  return read_buf;
}
```

### `Adafruit_BusIO` style **synchronous** threadsafe `Wire` access
([`examples/Threadsafe_IO/Wire_BusIO`](../examples/Threadsafe_IO/Wire_BusIO))

For a further simplification [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO) style APIs are provided:
```C++
byte lsm6dsox_read_reg(byte reg_addr)
{
  byte read_buf = 0;
  lsm6dsox.wire().write_then_read(&reg_addr, 1, &read_buf, 1);
  return read_buf;
}
```
