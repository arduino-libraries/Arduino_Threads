<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

Threading Basics
================
## Introduction
Previously Arduino sketches didn't support the concept of multitasking, unless you took specific measures to support it. With this so called single-threaded approach instructions in your Arduino sketch are executed one after another. You may have encountered the limitations of this when interacting with multiple sensors and actuators at once. For example if you let a servo motor react to the data read from a distance sensor. While the servo motor is moving to its target position no further reading of the distance sensor can be done because the program waits until the servo is done moving. To solve this issue multitasking can be used which allows to "simultaneously" execute multiple task such as reading from a sensor and controlling a motor.

In the historic single-threaded execution of Arduino sketches the complete program logic is contained within the `*.ino` file. It contains both a `setup()` function, which is executed only once at program start, and a `loop()` function, which is executed indefinitely. 
In order to support multi-threaded (or parallel) sketch execution a new file type called the `*.inot` file is introduced. While an Arduino project can contain multiple `*.ino` files you can define `setup()` or `loop()` only once among those <!-- TODO: What's a use case for using multiple ino files? -->. However, an Arduino project can contain multiple `*.inot` files. Each `*.inot` file represents a separate thread and contains it's own `setup()` and `loop()` functions. Consequently each one of the `*.inot` files can take up a specific task that they perform independent from each other. To get back to our above mentioned example, the program in one `*.inot` file could read from the distance sensor, while the one in the other file could take care of controlling the servo motor. <!-- TODO: Would this example actually work? -->

<!-- TODO: Add graphic of ino and inot file "hierarchy" -->

The advantage of this approach is that a complex and lengthy `loop()` function (potentially consisting of nested [finite state machines](https://en.wikipedia.org/wiki/Finite-state_machine)) found in typical Arduino sketches can be broken up into several, parallelly executed `loop()` functions with a much smaller scope. This not only increases program readability and maintainability but as a result leads to a reduction of software errors (bugs).

#### Example (Single-Threaded):
This sketch demonstrates how one would implement a program which requires the execution of three different actions on three different periodic intervals. In this example we blink three different LEDs at three different intervals.

**Blink_Three_LEDs.ino**:

```C++
void setup()
{
  pinMode(LED_RED,   OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE,  OUTPUT);
}

int const DELAY_RED_msec   = 900;
int const DELAY_GREEN_msec = 500;
int const DELAY_BLUE_msec  = 750;

void loop()
{
  static unsigned long prev_red   = millis();
  static unsigned long prev_green = millis();
  static unsigned long prev_blue  = millis();

  unsigned long const now = millis();

  if ((now - prev_red) > DELAY_RED_msec) {
    prev_red = now;
    digitalWrite(LED_RED, !digitalRead(LED_RED));
  }

  if ((now - prev_green) > DELAY_GREEN_msec) {
    prev_green = now;
    digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
  }

  if ((now - prev_blue) > DELAY_BLUE_msec) {
    prev_blue = now;
    digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
  }
}
```
You can imagine that with increasing complexity of a sketch it gets quite difficult to keep track of the different states used for the different sub-tasks (here: blinking each of the LEDs).

#### Example (Multi-Threaded):

The same functionality can be provided via multi-threaded execution in a much cleaner way.

**Blink_Three_LEDs.ino**

```C++
void setup() {
  // Start the task defined in the corresponding .inot file
  LedRed.start();
  LedGreen.start();
  LedBlue.start();
}

void loop() {
}
```
**LedRed.inot**
```C++
void setup() {
  pinMode(LED_RED, OUTPUT);
}

int const DELAY_RED_msec = 900;

void loop() {
  digitalWrite(LED_RED, !digitalRead(LED_RED));
  delay(DELAY_RED_msec);
}
```
**LedGreen.inot**
```C++
void setup() {
  pinMode(LED_GREEN, OUTPUT);
}

int const DELAY_GREEN_msec = 500;

void loop() {
  digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
  delay(DELAY_GREEN_msec);
}
```
**LedBlue.inot**
```C++
void setup() {
  pinMode(LED_BLUE, OUTPUT);
}

int const DELAY_BLUE_msec = 750;

void loop() {
  digitalWrite(LED_BLUE, !digitalRead(LED_BLUE));
  delay(DELAY_BLUE_msec);
}
```
As you can see from the example the name of the `*.inot`-file is used to generate a class and instantiate an object with the **same name** as the `*.inot`-file. Hence the `*.inot`-file can be only named in concordance with the rules to declare a variable in C++.  `*.inot`-file names:
* must begin with a letter of the alphabet or an underscore (_).
* can contain letters and numbers after the first initial letter.
* are case sensitive.
* cannot contain spaces or special characters.
* cannot be a C++ keyword (i.e. `register`, `volatile`, `while`, etc.).

To be consistent with the Arduino programming style we recommend using [camel case](https://en.wikipedia.org/wiki/Camel_case) for the file names.