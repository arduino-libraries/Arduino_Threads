`Arduino_Threads`
=================

[![Compile Examples status](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/compile-examples.yml/badge.svg)](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/compile-examples.yml)
[![Check Arduino status](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/check-arduino.yml/badge.svg)](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/check-arduino.yml)
[![Spell Check status](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/spell-check.yml/badge.svg)](https://github.com/arduino-libraries/Arduino_Threads/actions/workflows/spell-check.yml)

This library makes it easy to use the multi-threading capability of [Arduino](https://www.arduino.cc/) boards that use an [Mbed OS](https://os.mbed.com/docs/mbed-os/latest/introduction/index.html)-based core library.

## :zap: Features
### :thread: Multi-threaded sketch execution
Instead of one big state-machine-of-doom you can split your application into multiple independent threads, each with it's own `setup()` and `loop()` function. Instead of implementing your application in a single `.ino` file, each independent thread is implemented in a dedicated `.inot` representing a clear separation of concerns on a file level.

### :calling: Easy communication between multiple threads
Easy inter-thread-communication is facilitated via a `Shared` abstraction providing thread-safe sink/source semantics allowing to safely exchange data of any type between threads.

## :mag_right: Resources

* [How to install a library](https://www.arduino.cc/en/guide/libraries)
* [Help Center](https://support.arduino.cc/)
* [Forum](https://forum.arduino.cc)

## :bug: Bugs & Issues

If you want to report an issue with this library, you can submit it to the [issue tracker](issues) of this repository. Remember to include as much detail as you can about your hardware set-up, code and steps for reproducing the issue. Make sure you're using an original Arduino board.

## :technologist: Development

There are many ways to contribute:

* Improve documentation and examples
* Fix a bug
* Test open Pull Requests
* Implement a new feature
* Discuss potential ways to improve this library

You can submit your patches directly to this repository as Pull Requests. Please provide a detailed description of the problem you're trying to solve and make sure you test on real hardware.

## :yellow_heart: Donations

This open-source code is maintained by Arduino with the help of the community. We invest a considerable amount of time in testing code, optimizing it and introducing new features. Please consider [donating](https://www.arduino.cc/en/donate/) or [sponsoring](https://github.com/sponsors/arduino) to support our work, as well as [buying original Arduino boards](https://store.arduino.cc/) which is the best way to make sure our effort can continue in the long term.
