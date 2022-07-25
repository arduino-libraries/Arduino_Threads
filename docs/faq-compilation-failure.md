<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

FAQ: Known issues of language constructs within `inot`-files and possible workarounds
=====================================================================================
Encapsulating the .inot file content within a class prohibits a wide range of typical C/C++ constructs. By encapsulating the .inot file within a namespace instead those issues can be prevented (see https://github.com/arduino-libraries/Arduino_Threads/pull/47). While this PR is not yet merged please refer to this document describing possible work-arounds.

### No forward declaration of functions
```C++
int myFunc(int const a, int const b);

void setup()
{

}

void loop()
{
  static int c = 0;
  c += myFunc(0,c);
}

int myFunc(int const a, int const b)
{
  return (a + b);
}
```
This fails to compile because it doesn't look like a declaration and a definition but rather like an attempt to overload myFunc with the same signature (again) resulting in a compile error.

To fix this in the "class-wrapping" method, don't write forward declarations.
```diff
- int myFunc(int const a, int const b);
```
or declare them `static`. Note: This may have unintended side effects.
```diff
- int myFunc(int const a, int const b);
+ static int myFunc(int const a, int const b);
```

### Beware of the right function signature for callback functions
```C++
void myEventHandler()
{
  /* Do something. */
}

void setup()
{
  attachInterrupt(digitalPinToInterrupt(2), myEventHandler, CHANGE);
}

void loop()
{

}
```
This fails to compile because myEventHandler has the function signature of `void Thread::myEventHandler(void)` and is a member function of `class Thread` while `attachInterrupt` expects a function with the signature `void myEventHandler(void)`.

To fix this in the "class-wrapping" method, add `static` in front of the callback declaration.
```diff
- void myEventHandler()
+ static void myEventHandler()
```

### No in-class-initialisation of a static member variables
```C++
static int my_global_variable = 0;

void setup()
{

}

void loop()
{

}
```
This fails to compile because in-class-initialisation of a static member variable is forbidden.

To fix this in the "class-wrapping" method, remove `static` or remove the initialization.
```diff
- static int my_global_variable = 0;
+ int my_global_variable = 0;
```
or
```diff
- static int my_global_variable = 0;
+ static int my_global_variable;

void setup()
{
  my_global_variable = 0;
}
```

### Work-arounds for object instantiation in inot-files.
```C++
BusDevice lsm6dsox(Wire, LSM6DSOX_ADDRESS);

byte lsm6dsox_read_reg(byte reg_addr)
{
  byte read_buf = 0;
  lsm6dsox.wire().write_then_read(&reg_addr, 1, &read_buf, 1);
  return read_buf;
}
```
A possible work-around is
```diff
- BusDevice lsm6dsox(Wire, LSM6DSOX_ADDRESS);
+ BusDevice lsm6dsox = BusDevice{Wire, LSM6DSOX_ADDRESS};
```

### Functions need to be declared before calling them
```C++
void setup()
{
  attachInterrupt(digitalPinToInterrupt(2), myEventHandler, CHANGE);
}

void loop()
{

}

static void myEventHandler()
{
  /* Do something. */
}
```
This fails to compile because `myEventHandler` is declared after `setup`/`loop` and currently there's not automatic prototype generation as done for the ino file (Note: Therefor it's a very easy trap to fall prey too).

A possible work-around is to move the declaration before the actual usage:
```diff
+ static void myEventHandler()
+ {
+   /* Do something. */
+ }


void setup()
{
  attachInterrupt(digitalPinToInterrupt(2), myEventHandler, CHANGE);
}

void loop()
{

}

- static void myEventHandler()
- {
-   /* Do something. */
- }
```
