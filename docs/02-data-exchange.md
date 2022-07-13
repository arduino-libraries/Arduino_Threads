<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

Data Exchange between Threads
=============================
## Introduction
When an Arduino sketch formerly consisting of just a single `loop()` is split into multiple `loop()` functions contained in multiple `*.inot` files it becomes necessary to define a thread-safe mechanism to exchange data between those threads. For example, one `*.inot` file could calculate a math formula and send the result back to the main `*.ino` file to act upon it. Meanwhile the main `*.ino` file can take care of other tasks.

 `Arduino_Threads` supports two different mechanisms for data exchange between threads: `Shared` variables and `Sink`/`Source` semantics. Both have their pros and cons as described below.

## `Shared`
A `Shared` variable is a global variable accessible to all threads. It can be declared within a header file named `SharedVariables.h` which is automatically included at the top of each `*.inot`-file :
```C++
/* SharedVariables.h */
SHARED(counter, int); /* A globally available, threadsafe, shared variable of type 'int'. */
/* ... or ... */
SHARED(counter, int, 8); /* Same as before, but now the internal queue size is defined as 8. */
```
Writing to and reading from the shared variable may not always happen concurrently. I.e. a thread reading sensor data may update the shared variable faster than a slower reader thread would extract those values. Therefore the shared variable is modeled as a queue which can store (buffer) a certain number of entries. That way the slower reader thread can access all the values in the same order as they have been written.
New values can be inserted by using the `push` function that you may know from other data structures.

```C++
/* Thread_1.inot */
counter.push(10); /* Store a value into the shared variable in a threadsafe manner. */
```
If the internal queue is full the oldest element is discarded and the latest element is inserted into the queue.

Stored data can be retrieved by using the `pop` function:
```C++
/* Thread_2.inot */
Serial.println(counter.pop()); /* Retrieves a value from the shared variable in a threadsafe manner. */
```

Should the internal queue be empty when trying to read the latest available value then the thread reading the shared variable will be suspended and the next available thread will be scheduled. Once a new value is stored inside the shared variable the suspended thread resumes operation and consumes the value which has been stored in the internal queue.
Since shared variables are globally accessible from every thread, each thread can read from or write to the shared variable. The user is responsible for using the shared variable in a responsible and sensible way, i.e. reading a shared variable from different threads is generally a bad idea, as on every read an item is removed from the queue within the shared variable and other threads can't access the read value anymore .

## `Sink`/`Source`
The idea behind the `Sink`/`Source` semantics is to model data exchange between one data producer (`Source`) and one or multiple data consumers (`Sink`). A data producer or `Source` can be declared in any `*.ino` or `*.inot`-file using the `SOURCE` macro:
```C++
/* DataProducerThread.inot */
SOURCE(counter, int); /* Declaration of a data source of type `int`. */
```
In a similar way, a data consumer can be declared in any `*.ino` or `*.inot`-file using the `SINK` macro. In difference to `Shared` where the size of the internal queue is globally set for all shared variables you can define your desired internal buffer size separately for each `Sink`.
```C++
/* DataConsumerThread_1.inot */
SINK(counter, int); /* Declaration of a data sink of type `int` with a internal queue size of '1'. */
```
```C++
/* DataConsumerThread_2.inot */
SINK(counter, int, 10); /* Declaration of a data sink of type `int` with a internal queue size of '10'. */
```
In order to actually facilitate the flow of data from a source to a sink the sinks must be connected to the desired data source. This is done within the main `ino`-file:
```C++
/* MySinkSourceDemo.ino */
DataProducerThread.counter.connectTo(DataConsumerThread_1.counter);
DataProducerThread.counter.connectTo(DataConsumerThread_2.counter);
```
Whenever a new value is assigned to a data source, i.e.
```C++
/* DataProducerThread.inot */
counter.push(10);
```
data is automatically copied and stored within the internal queues of all connected data sinks, from where it can be retrieved, i.e.
```C++
/* DataConsumerThread_1.inot */
Serial.println(counter.pop());
```
```C++
/* DataConsumerThread_2.inot */
Serial.println(counter.pop());
```
If a thread tries to read from an empty `Sink` the thread is suspended and the next ready thread is scheduled. When a new value is written to a `Source` and consequently copied to a `Sink` the suspended thread is resumed and continuous execution (i.e. read the data and act upon it).

Since the data added to the source is copied multiple threads can read data from a single source without data being lost. This is an advantage compared to a simple shared variable. Furthermore you cannot accidentally write to a `Sink` or read from a `Source`. Attempting to do so results in a compilation error.

## Comparison
|  | :+1: | :-1: |
|:---:|:---:|:---:|
| `Shared` | :+1: Needs to be declared only once (in `SharedVariables.h`). | :-1: Basically a global variable, with all the disadvantages those entail.<br/> :-1: Size of internal queue fixed for ALL shared variables.<br/> :-1: No protection against misuse (i.e. reading from multiple threads).<br/> |
| `Sink`/`Source` | :+1: Define internal queue size separately for each `Sink`.<br/> :+1: Supports multiple data consumers for a single data producer.<br/> :+1: Read/Write protection: Can't read from `Source`, can't write to `Sink`.<br/> :+1: Mandatory connecting (plumbing) within main `*.ino`-file makes data flows easily visible.<br/> | :-1: Needs manual connection (plumbing) to connect `Sink`'s to `Source`'s. |
