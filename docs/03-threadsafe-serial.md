<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

Threadsafe `Serial`
===================
## Introduction
While both `SPI` and `Wire` are communication protocols which explicitly exist to facilitate communication between one server device and multiple client devices there are no such considerations for the `Serial` interface. `Serial` communication usually exists in a one-to-one mapping between two communication partners of equal power (both can initiate communication on their own right, there is no server/client relationship).

One example would be an Arduino sketch sending AT commands to a modem and interpreting the result of those commands. Another example would be a GPS unit sending NMEA encoded location data to the Arduino for parsing. In both cases the only sensible software representation for such functionality (AT command module or NMEA message parser) is a single thread. Also in both cases it is undesirable for other threads to inject other kind of data into the serial communication as this would only confuse i.e. the AT controlled modem. A good example for multiple threads writing to serial would be logging. A plausible example for multiple threads reading from `Serial` would be to i.e. split the NMEA parser across multiple threads, i.e. one thread only parses RMC-messages, another parses GGA-messages, etc. In any case the threadsafe `Serial` has to both support single-writer/single-reader and multiple-write/multiple-reader scenarios.

## Initialize Serial with `begin()`
In order to initialize the serial interface for any given thread `Serial.begin(baudrate)` needs to be called in any thread's `setup()` which desires to have **writing** access to the serial interface. Since it does not make sense to support multiple different baudrates (i.e. Thread_A writing with 9600 baud and Thread_B writing with 115200 baud - if you really need this, spread the attached serial devices to different serial ports), the first thread to call `Serial.begin()` locks in the configuration for all other threads. A sensible approach is to call `Serial.begin()` within the main `*.ino`-file and only then start the other threads, i.e.
```C++
/* MyThreadsafeSerialDemo.ino */
void setup()
{
  Serial.begin(9600);
  while (!Serial) { }
  /* ... */
  Thread_1.start();
  Thread_2.start();
  /* ... */
}
```
```C++
/* Thread_1.inot */
void setup() {
  Serial.begin(9600);
}
```
```C++
/* Thread_2.inot */
void setup() {
  Serial.begin(9600);
}
```

## Write to Serial with `print()`/`println()`
([`examples/Threadsafe_IO/Serial_Writer`](../examples/Threadsafe_IO/Serial_Writer))

Since the threadsafe `Serial` is derived from [`arduino::HardwareSerial`](https://github.com/arduino/ArduinoCore-API/blob/master/api/HardwareSerial.h) it does support the full range of the usual `Serial` API. This means that you can simply write to the `Serial` interface you've been using with the single threaded application.
```C++
Serial.print("This is a test message #");
Serial.println(counter);
```

### Prevent message break-up using `block()`/`unblock()`
([`examples/Threadsafe_IO/Serial_Writer`](../examples/Threadsafe_IO/Serial_Writer))

Due to the pre-emptive nature of the underlying mbed-os a multi-line `Serial.print/println()` could be interrupted at any point in time. When multiple threads are writing to the Serial interface, this can lead to jumbled-up messages.
```C++
/* Thread_1.inot */
void loop() {
  Serial.print("This ");
  Serial.print("is ");
  Serial.print("a ");
  Serial.print("multi-line ");
  Serial.print("log ");
  /* Interruption by scheduler and context switch. */
  Serial.print("message ");
  Serial.print("from ");
  Serial.print("thread #1.");
  Serial.println();
}
```
```C++
/* Thread_2.inot */
void loop() {
  Serial.print("This ");
  Serial.print("is ");
  Serial.print("a ");
  Serial.print("multi-line ");
  Serial.print("log ");
  Serial.print("message ");
  Serial.print("from ");
  Serial.print("thread #2.");
  Serial.println();
}
```
The resulting serial output of `Thread_1` being interrupted at the marked spot and `Thread_2` being scheduled can be seen below:
```bash
This is a multi-line log This is a multi-line log message from thread #2.
message from thread #1.

```
In order to prevent such break-ups a `block()`/`unblock()` API is introduced which ensures that the messages are printed in the intended order, i.e.
```C++
/* Thread_1.inot */
void loop() {
  Serial.block();
  Serial.print("This ");
  Serial.print("is ");
  /* ... */
  Serial.print("thread #1.");
  Serial.println();
  Serial.unblock();
}
```
```C++
/* Thread_2.inot */
void loop() {
  Serial.block();
  Serial.print("This ");
  Serial.print("is ");
  /* ... */
  Serial.print("thread #2.");
  Serial.println();
  Serial.unblock();
}
```
Now the thread messages are printed in the order one would expect:
```bash
This is a multi-line log message from thread #2.
This is a multi-line log message from thread #1.
```

## Read from `Serial`
([`examples/Threadsafe_IO/Serial_Reader`](../examples/Threadsafe_IO/Serial_Reader))

Reading from the `Serial` interface can be accomplished using the `read()`, `peek()` and `available()` APIs.
```C++
/* Thread_1.inot */
void loop()
{
  /* Read data from the serial interface into a String. */
  String serial_msg;
  while (Serial.available())
    serial_msg += (char)Serial.read();
  /* ... */
}
```
Whenever a thread first call any of those three APIs a thread-dedicated receive ringbuffer is created and any incoming serial communication from that point on is copied in the threads dedicated receive ringbuffer. Having a dedicated receive ringbuffer per thread prevents "data stealing" from other threads in a multiple reader scenario, where the first thread to call `read()` would in fact receive the data and all other threads would miss out on it.
