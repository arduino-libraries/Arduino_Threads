<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right" />

`Arduino_Threads`
=================
*Note: This library is currently in [beta](#zap-caveats).*

[![Compile Examples status](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/compile-examples.yml/badge.svg)](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/compile-examples.yml)
[![Check Arduino status](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/check-arduino.yml/badge.svg)](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/check-arduino.yml)
[![Spell Check status](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/spell-check.yml/badge.svg)](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/spell-check.yml)

This library makes it easy to use the multi-threading capability of [Arduino](https://www.arduino.cc/) boards that use an [Mbed OS](https://os.mbed.com/docs/mbed-os/latest/introduction/index.html)-based core library. Additionally this library provides thread-safe access to `Wire`, `SPI` and `Serial` which is relevant when creating multi-threaded sketches in order to avoid common pitfalls such as race-conditions and invalid state. ​

Preeliminary **documentation** and download links for **required tooling** are available within the [`/docs`](docs/README.md) subfolder.

## :star: Features
### :thread: Multi-threaded sketch execution
Instead of one big state-machine-of-doom you can split your application into multiple independent threads, each with it's own `setup()` and `loop()` function. Instead of implementing your application in a single `.ino` file, each independent thread is implemented in a dedicated `.inot` file (t suffix stands for **t**hread) representing a clear separation of concerns on a file level.

### :calling: Easy communication between multiple threads
Easy inter-thread-communication is facilitated via a `Shared` abstraction object providing thread-safe sink/source semantics allowing to safely exchange data of any type between threads.

### :shield: Thread-safe I/O
A key problem of multi-tasking is the **prevention of erroneous state when multiple threads share a single resource**. The following example borrowed from a typical application demonstrates the problems resulting from multiple threads accessing a single resource:

Imagine an embedded system where multiple `Wire` client devices are physically connected to a single `Wire` server. Each `Wire` client device is managed by a separate software thread. Each thread polls its `Wire` client device periodically. Access to the I<sup>2</sup>C bus is managed via the `Wire` library and typically follows this pattern:

```C++
/* Wire Write Access */
Wire.beginTransmission(address);
Wire.write(value);
// Interrupting the current thread e.g. at this point can lead to an erroneous state
// if another thread performs Wire I/O before the transmission ends.
Wire.endTransmission();

/* Wire Read Access */
Wire.requestFrom(address, bytes)
while(Wire.available()) {
  int value = Wire.read();
}
```

Since we are using [ARM Mbed OS](https://os.mbed.com/mbed-os/) which is a [preemptive](https://en.wikipedia.org/wiki/Preemption_(computing)) [RTOS](https://en.wikipedia.org/wiki/Real-time_operating_system) for achieving multi-tasking capability and under the assumption that all threads share the same priority (which leads to a [round-robin](https://en.wikipedia.org/wiki/Round-robin_scheduling) scheduling) it can easily happen that one thread is half-way through its Wire I/O access when the scheduler interrupts its execution and schedules the next thread which in turn starts, continues or ends its own Wire I/O access.

As a result this interruption by the scheduler will break Wire I/O access for both devices and leave the Wire I/O controller in an undefined state :fire:.

`Arduino_Threads` solves this problem by encapsulating the complete I/O access (e.g. reading from a `Wire` client device) within a single function call which generates an I/O request to be asynchronously executed by a high-priority I/O thread. The high-priority I/O thread is the **only** instance which directly communicates with physical hardware.

### :runner: Asynchronous
The mechanisms implemented in this library allow any thread to dispatch an I/O request asynchronously and either continue its operation or [yield](https://en.wikipedia.org/wiki/Yield_(multithreading)) control to the next scheduled thread. All I/O requests are stored in a queue and are executed within a high-priority I/O thread after a [context-switch](https://en.wikipedia.org/wiki/Context_switch). An example of this can be seen [here](examples/Threadsafe_IO/Threadsafe_SPI/Threadsafe_SPI.ino).

### :relieved: Convenient API
Although you are free to directly manipulate I/O requests and responses (e.g. [Threadsafe_Wire](examples/Threadsafe_IO/Threadsafe_Wire/Threadsafe_Wire.ino)) there are convenient `read`/`write`/`writeThenRead` abstractions inspired by the [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO) library (e.g. [Threadsafe_Wire_BusIO](examples/Threadsafe_IO/Threadsafe_Wire_BusIO/Threadsafe_Wire_BusIO.ino)).

## :zap: Caveats

This library is currently in **BETA** phase. This means that neither the API nor the usage patterns are set in stone and are likely to change. We are publishing this library in the full knowledge that we can't foresee every possible use-case and edge-case. Therefore we would like to treat this library, while it's in beta phase, as an experiment and ask for your input for shaping this library. Please help us by providing feedback in the [issues section](https://github.com/arduino-libraries/Arduino_Threads/issues) or participating in our [discussions](https://github.com/arduino-libraries/Arduino_Threads/discussions).

## :mag_right: Resources

* [How to install a library](https://www.arduino.cc/en/guide/libraries)
* [Help Center](https://support.arduino.cc/) - Get help from Arduino's official support team
* [Forum](https://forum.arduino.cc) - Get support from the community

## :bug: Bugs & Issues

If you found an issue in this library, you can submit it to the [issue tracker](issues) of this repository. Remember to include as much detail as you can about your hardware set-up, code and steps for reproducing the issue. To prevent hardware related incompatibilities make sure to use an [original Arduino board](https://support.arduino.cc/hc/en-us/articles/360020652100-How-to-spot-a-counterfeit-Arduino).

## :technologist: Contribute

There are many ways to contribute:

* Improve documentation and examples
* Fix a bug
* Test open Pull Requests
* Implement a new feature
* Discuss potential ways to improve this library

You can submit your patches directly to this repository as Pull Requests. Please provide a detailed description of the problem you're trying to solve and make sure you test on real hardware.

## :yellow_heart: Donations

This open-source code is maintained by Arduino with the help of the community. We invest a considerable amount of time in testing code, optimizing it and introducing new features. Please consider [donating](https://www.arduino.cc/en/donate/) or [sponsoring](https://github.com/sponsors/arduino) to support our work, as well as [buying original Arduino boards](https://store.arduino.cc/) which is the best way to make sure our effort can continue in the long term.
