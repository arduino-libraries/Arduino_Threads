<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

Thread-safe `Wire`
=================
## Introduction
A common problem of multi-tasking is the prevention of erroneous state when multiple threads share a single resource. The following example borrowed from a typical application demonstrates these problems:

Imagine an embedded system where multiple `Wire` client devices are physically connected to a single `Wire` server. Each `Wire` client device is managed by a dedicated software thread. Each thread polls its `Wire` client device periodically. Access to the `Wire` bus is managed via the `Wire` library and typically follows the pattern described below:
```C++
/* Wire Write Access */
Wire.beginTransmission(address);
Wire.write(value);
Wire.endTransmission();

/* Wire Read Access */
Wire.requestFrom(address, bytes)
while(Wire.available()) {
  int value = Wire.read();
}
```
Since we are using the [preemptive](https://os.mbed.com/docs/mbed-os/v6.11/program-setup/concepts.html#threads) [RTOS](https://en.wikipedia.org/wiki/Real-time_operating_system) [ARM Mbed OS](https://os.mbed.com/mbed-os/) with a tick time of 10 ms for achieving multi-tasking capability and under the assumption that all threads share the same priority (which leads to a [round-robin scheduling](https://en.wikipedia.org/wiki/Round-robin_scheduling)) it can easily happen that one thread is half-way through its `Wire` access when the scheduler interrupts it and schedules the next thread which in turn starts/continues/ends its own `Wire` access.

As a result this interruption by the scheduler will break `Wire` access for both devices and leave the `Wire` controller in an undefined state. 

In Arduino Parallela we introduced the concept of `BusDevice`s which are meant to unify the way sketches access peripherals through heterogeneous busses such as `Wire`, `SPI` and `Serial`. A `BusDevice` is declared simply by specifying the type of interface and its parameters: 
```C++
BusDevice lsm6dsox(Wire, LSM6DSOX_ADDRESS);
/* or */
BusDevice lsm6dsox(Wire, LSM6DSOX_ADDRESS, true /* restart */);
/* or */
BusDevice lsm6dsox(Wire, LSM6DSOX_ADDRESS, false /* restart */, true, /* stop */);
```

### Asynchronous thread-safe `Wire` access with `transfer`/`wait` 
Once a `BusDevice` is declared it can be used to transfer data to and from the peripheral by means of the `transfer()` API. As opposed to the traditional Arduino bus APIs, `transfer()` is asynchronous and thus won't block execution unless the `wait()` function is called.
Note that we are in a parallel programming environment which means that calls to `transfer()` on the same bus from different sketches will be arbitrated.

```C++
byte lsm6dsox_read_reg(byte const reg_addr)
{
  byte write_buffer = reg_addr;
  byte read_buffer  = 0;
  
  IoRequest  request(write_buffer, read_buffer);
  IoResponse response = lsm6dsox.transfer(request);
  
  /* Wait for the completion of the IO Request. 
     Allows other threads to run */
  response->wait();
 
  return read_buffer;
}
```

### Synchronous thread-safe `Wire` access with `transferAndWait` 
([`examples/Threadsafe_IO/Wire`](../examples/Threadsafe_IO/Wire))

As the use of the `transfer` API might be difficult to grasp there's also a synchronous API call combining the request of the transfer and waiting for its result using `transferAndWait`. 
```C++
byte lsm6dsox_read_reg(byte const reg_addr)
{
  byte write_buffer = reg_addr;
  byte read_buffer  = 0;
  
  IoRequest  request(write_buffer, read_buffer);
  IoResponse response = transferAndWait(lsm6dsox, request); /* Transmit IO request for execution and wait for completion of request. */
  
  return read_buffer;
}
```

### `Adafruit_BusIO` style **synchronous** thread-safe `Wire` access
([`examples/Threadsafe_IO/Wire_BusIO`](../examples/Threadsafe_IO/Wire_BusIO))

For further simplification [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO) style APIs are provided:

```C++
byte lsm6dsox_read_reg(byte reg_addr)
{
  byte read_buffer = 0;
  lsm6dsox.wire().writeThenRead(&reg_addr, 1, &read_buffer, 1);
  return read_buffer;
}
```
