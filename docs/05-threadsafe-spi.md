<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

Threadsafe `SPI`
===============
## Introduction
`SPI` communication shares the same problems that [have been demonstrated](04-threadsafe-wire.md) for using `Wire` in a multithreaded environment. This is due to the fact that every `SPI` transaction consists of multiple function calls, i.e.
```C++
digitalWrite(cs, LOW);
SPI.beginTransaction(SPI_SETTINGS);
rx = SPI.transfer(tx);
SPI.endTransaction();
digitalWrite(cs, HIGH);
```
Due to the preemptive nature of the underlying RTOS the execution of the `SPI` transaction can be interrupted at any point in time. This would leave both the `SPI` driver code and the client device in a undefined state. Similar as has been done for `Wire` this is solved by introducing a `BusDevice` which allows for single-function call, threadsafe, atomic SPI transactions. A `BusDevice` is declared simply by specifying the type of interface and its parameters:
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

### `transfer`/`wait` **asynchronous** threadsafe `SPI` access
Once a `BusDevice` is declared it can be used to transfer data to and from the peripheral by means of the `transfer()` API. As opposed to traditional Arduino bus APIs `transfer()` is asynchronous and thus won't block execution unless the `wait()` function is called.
Note that since we are in a parallel programming environment this means that calls to `transfer()` on the same bus from different sketches will be arbitrated and that the `wait()` API will suspend the sketch until the transfer is complete, thus allowing other processes to execute or going to low power state.
```C++
byte bmp388_read_reg(byte const reg_addr)
{
  /* REG_ADDR | DUMMY_BYTE | REG_VAL is on SDO */
  byte read_write_buf[] = {static_cast<byte>(0x80 | reg_addr), 0, 0};

  IoRequest req(read_write_buf, sizeof(read_write_buf), nullptr, 0);
  IoResponse rsp = bmp388.transfer(req);
  /* Do other stuff */
  rsp->wait(); /* Wait for the completion of the IO Request. */
 
  return read_write_buf[2];
}
```

### `transfer_and_wait` **synchronous** threadsafe `SPI` access
([`examples/Threadsafe_IO/SPI`](../examples/Threadsafe_IO/SPI))

As the use of the `transfer` API might be confusing there's also a synchronous API call combining the request of the transfer and waiting for it's result using `transfer_and_wait`.
```C++
byte bmp388_read_reg(byte const reg_addr)
{
  /* REG_ADDR | DUMMY_BYTE | REG_VAL is on SDO */
  byte read_write_buf[] = {static_cast<byte>(0x80 | reg_addr), 0, 0};

  IoRequest req(read_write_buf, sizeof(read_write_buf), nullptr, 0);
  IoResponse rsp = transfer_and_wait(bmp388, req);

  return read_write_buf[2];
}
```

### `Adafruit_BusIO` style **synchronous** threadsafe `SPI` access
([`examples/Threadsafe_IO/SPI_BusIO`](../examples/Threadsafe_IO/SPI_BusIO))

For a further simplification [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO) style APIs are provided:
```C++
byte bmp388_read_reg(byte const reg_addr)
{
  /* REG_ADDR | DUMMY_BYTE | REG_VAL is on SDO */
  byte write_buf[2] = {static_cast<byte>(0x80 | reg_addr), 0};
  byte read_buf = 0;

  bmp388.spi().write_then_read(write_buf, sizeof(write_buf), &read_buf, sizeof(read_buf));
  return read_buf;
}
```
