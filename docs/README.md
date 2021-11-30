<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

`Arduino_Threads/docs`
======================
The Arduino threading APIs brings multi-threading to the world of Arduino.

The following Arduino architectures and boards are supported:
* `mbed_portenta`: [Portenta H7](https://store.arduino.cc/products/portenta-h7)
* `mbed_nano`: [Nano 33 BLE](https://store.arduino.cc/arduino-nano-33-ble), [Nano RP2040 Connect](https://store.arduino.cc/nano-rp2040-connect)
* `mbed_edge`: [Edge Control](https://store.arduino.cc/products/arduino-edge-control)

Threading on Arduino can be achieved by leveraging the [Arduino_Threads](https://github.com/bcmi-labs/Arduino_Threads) library in combination with [arduino-cli](https://github.com/facchinm/arduino-cli/commits/arduino_threads_rebased).

Download pre-built `arduino-cli`:
* [MacOS_64Bit](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_macOS_64bit.tar.gz)
* [Linux_32Bit](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Linux_32bit.tar.gz)
* [Linux_64Bit](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Linux_64bit.tar.gz)
* [Linux_ARMv6](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Linux_ARMv6.tar.gz)
* [Linux_ARMv7](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Linux_ARMv7.tar.gz)
* [Windows_32Bit](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Windows_32bit.zip)
* [Windows_64Bit](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Windows_64bit.zip)

### Table of Contents
* [Threading Basics](01-threading-basics.md)
* [Data exchange between threads](02-data-exchange.md)
* [Threadsafe `Serial`](03-threadsafe-serial.md)
* [Threadsafe `Wire`](04-threadsafe-wire.md)
* [Threadsafe `SPI`](05-threadsafe-spi.md)
