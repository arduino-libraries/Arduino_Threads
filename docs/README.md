<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

`Arduino_Threads/docs`
======================
The Arduino threading APIs brings multi-threading to the world of Arduino. If you're new to the concept of threads we suggest you have first a look at the [Threading Basics](threading-basics.md) document.

The following Arduino architectures and boards are currently supported:
* `mbed_portenta`: [Portenta H7](https://store.arduino.cc/products/portenta-h7)
* `mbed_nano`: [Nano 33 BLE](https://store.arduino.cc/arduino-nano-33-ble), [Nano RP2040 Connect](https://store.arduino.cc/nano-rp2040-connect)
* `mbed_edge`: [Edge Control](https://store.arduino.cc/products/arduino-edge-control)
* `mbed_nicla`: [Nicla Sense ME](https://store.arduino.cc/products/nicla-sense-me), [Nicla Vision](http://store.arduino.cc/products/nicla-vision)

### Tooling

Threading with the above mentioned Arduino cores can be achieved by leveraging the [Arduino_Threads](https://github.com/bcmi-labs/Arduino_Threads) library in combination with the [Arduino Command Line Interface](https://github.com/facchinm/arduino-cli/commits/arduino_threads_rebased) (arduino-cli).

For those who are unsure how to use the Arduino CLI in combination with multi-threading take a look a condensed [getting started guide](cli-getting-started.md). There's also the comprehensive [arduino-cli documentation](https://arduino.github.io/arduino-cli/getting-started) to consider.

Download a preliminary, pre-built `arduino-cli` binary (experimental) or patched `Arduino IDE` (very experimental) for your operating system that supports threading:
| Platform | Download CLI | Download IDE |
|:-:|:-:|:-:|
| MacOS_64Bit | [`arduino-cli`](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_macOS_64bit.tar.gz) | [`Arduino IDE`](https://downloads.arduino.cc/ide_staging/arduino_threads/arduino-PR-ae92bd4498e867b07581d1d4191be14b9ef0f69a-BUILD-65-macosx.zip) |
| Linux_32Bit | [`arduino-cli`](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Linux_32bit.tar.gz) | [`Arduino IDE`](https://downloads.arduino.cc/ide_staging/arduino_threads/arduino-PR-ae92bd4498e867b07581d1d4191be14b9ef0f69a-BUILD-65-linux32.tar.xz) |
| Linux_64Bit | [`arduino-cli`](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Linux_64bit.tar.gz) | [`Arduino IDE`](https://downloads.arduino.cc/ide_staging/arduino_threads/arduino-PR-ae92bd4498e867b07581d1d4191be14b9ef0f69a-BUILD-65-linux64.tar.xz) |
| Linux_ARMv6 | [`arduino-cli`](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Linux_ARMv6.tar.gz) | |
| Linux_ARMv7 | [`arduino-cli`](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Linux_ARMv7.tar.gz) | [`Arduino IDE`](https://downloads.arduino.cc/ide_staging/arduino_threads/arduino-PR-ae92bd4498e867b07581d1d4191be14b9ef0f69a-BUILD-65-linuxarm.tar.xz) |
| Windows_32Bit | [`arduino-cli`](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Windows_32bit.zip) | |
| Windows_64Bit | [`arduino-cli`](https://downloads.arduino.cc/tools/arduino-cli/inot_support/arduino-cli_git-snapshot_Windows_64bit.zip) | [`Arduino IDE`](https://downloads.arduino.cc/ide_staging/arduino_threads/arduino-PR-ae92bd4498e867b07581d1d4191be14b9ef0f69a-BUILD-65-windows.zip) |

### Table of Contents

In the following documents you will find more information about the topics covered by this library.

1. [Threading Basics](threading-basics.md)
2. [Data exchange between threads](data-exchange.md)
3. [Threadsafe `Serial`](threadsafe-serial.md)
4. [Threadsafe `Wire`](threadsafe-wire.md)
5. [Threadsafe `SPI`](threadsafe-spi.md)
