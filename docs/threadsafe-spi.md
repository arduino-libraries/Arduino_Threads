<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

Thread-safe `SPI`
===============
## Introduction
`SPI` communication shares the same problems that [have been outlined](threadsafe-wire.md) for using `Wire` in a multithreaded environment. This is due to the fact that every `SPI` transaction consists of multiple function calls, i.e.

```C++
digitalWrite(cs, LOW);
SPI.beginTransaction(SPI_SETTINGS);
rx = SPI.transfer(tx);
SPI.endTransaction();
digitalWrite(cs, HIGH);
```
Due to the preemptive nature of the underlying RTOS the execution of the `SPI` transaction can be interrupted at any point in time. This would leave both the `SPI` driver code and the client device in a undefined state. Similar to what has been implemented for `Wire`, Arduino Parallela solves this by introducing a `BusDevice` which allows for single-function calls. The Parallela API allows for thread-safe, atomic SPI transactions. A `BusDevice` is declared simply by specifying the type of the interface and its parameters:
```C++
int const DEVICE_CS_PIN = 4;
void device_cs_select() { /* ... */ }
void device_cs_deselect() { /* ... */ }
/* ... */
BusDevice bmp388(SPI, DEVICE_CS_PIN, spi_settings);
/* or */
BusDevice bmp388(SPI, DEVICE_CS_PIN, spi_clock, spi_bit_order, spi_bit_mode);
/* or */
BusDevice bmp388(SPI, device_cs_select, device_cs_deselect, spi_settings);
```

### Asynchronous thread-safe `SPI` access with `transfer`/`wait` 
Once a `BusDevice` is declared it can be used to transfer data to and from the peripheral by means of the `transfer()` API. As opposed to the traditional Arduino bus APIs, `transfer()` is asynchronous and thus won't block execution unless the `wait()` function is called.
Note that we are in a parallel programming environment which means that calls to `transfer()` on the same bus from different sketches will be arbitrated.

```C++
byte bmp388_read_reg(byte const reg_addr)
{
  /* REG_ADDR | DUMMY_BYTE | REG_VAL is on SDO */
  byte read_write_buffer[] = {0x80 | reg_addr, 0, 0};

  IoRequest request(read_write_buffer, sizeof(read_write_buffer), nullptr, 0);
  IoResponse response = bmp388.transfer(request);
  /* Do other stuff */
  response->wait(); /* Wait for the completion of the IO Request. */
  auto value = read_write_buffer[2]; 
  return value;
}
```

### Synchronous thread-safe `SPI` access with `transferAndWait` 
([`examples/Threadsafe_IO/SPI`](../examples/Threadsafe_IO/SPI))

As the use of the `transfer` API might be difficult to grasp there's also a synchronous API call combining the request of the transfer and waiting for its result using `transferAndWait`.
```C++
byte bmp388_read_reg(byte const reg_addr)
{
  /* REG_ADDR | DUMMY_BYTE | REG_VAL is on SDO */
  byte read_write_buffer[] = {0x80 | reg_addr, 0, 0};

  IoRequest request(read_write_buffer, sizeof(read_write_buffer), nullptr, 0);
  IoResponse response = transferAndWait(bmp388, request);

  auto value = read_write_buffer[2]; 
  return value;
}
```

### `Adafruit_BusIO` style **synchronous** thread-safe `SPI` access
([`examples/Threadsafe_IO/SPI_BusIO`](../examples/Threadsafe_IO/SPI_BusIO))

For a further simplification [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO) style APIs are provided:

```C++
byte bmp388_read_reg(byte const reg_addr)
{
  /* REG_ADDR | DUMMY_BYTE | REG_VAL is on SDO */
  byte write_buffer[2] = {0x80 | reg_addr, 0};
  byte read_buffer = 0;

  bmp388.spi().writeThenRead(write_buffer, sizeof(write_buffer), &read_buffer, sizeof(read_buffer));
  return read_buffer;
}
```
