<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

Data Exchange between Threads
=============================
## Introduction
When a Arduino sketch formerly consisting of a single `loop()` is split into multiple `loop()` functions contained in multiple `*.inot` files it becomes necessary to device a thead-safe mechanism to exchange data between those threads. `Arduino_Threads` supports two different semantics for data exchange between threads, `Shared` variables and `Sink`/`Source` semantic.

## `Shared`
A `Shared` variable is a global variable accessible to all threads. It is declared within `SharedVariables.h` which is automatically included on the top of each `*.inot`-file. Shared variables are declared using the `SHARED` macro in the following way:
```C++
/* SharedVariables.h */
SHARED(counter, int); /* A globally available, threadsafe, shared variable of type 'int'. */
```
A shared variable can contain up to `SHARED_QUEUE_SIZE` entries and semantically represents a FIFO ([First-In/First-Out](https://en.wikipedia.org/wiki/FIFO_(computing_and_electronics))) queue. New values can be inserted into the queue by naturally using the assignment operator `=` as if it was just any ordinary [plain-old-data](https://en.wikipedia.org/wiki/Passive_data_structure) (POD) type, i.e. `int`, `char`, ... 
```C++
/* Thread_1.inot */
counter = 10; /* Store a value into the shared variable in a threadsafe manner. */
```
If the internal queue is full the oldest element is discarded and the latest element is inserted into the queue.

Retrieving stored data works also very naturally like it would for any POD data type:
```C++
/* Thread_2.inot */
Serial.println(counter); /* Retrieves a value from the shared variable in a threadsafe manner. */
```
Should the internal queue be empty when trying to read the latest available value then the thread reading the shared variable will be suspended and the next available thread will be schedulded. Once a new value is stored inside the shared variable the suspended thread resumes operation and consumes the value which has been stored in the internal queue.

Since shared variables are globally accessible from every thread each thread can read from or write to the shared variable. The user is responsible for using the shared variable in a responsible and sensible way, i.e. reading a shared variable from different threads is generally a bad idea, as on every read a item is removed from the queue within the shared variable and other threads can't access the read value anymore.

## `Sink`/`Source`
The idea behind the `Sink`/`Source` semantics is to model data exchange between one data producer (`Source`) and zero, one or multiple data consumers (`Sink`). A data producer or `Source` can be declared in any `*.ino` or `*.inot`-file using the `SOURCE` macro:
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
counter = 10;
```
data is automatically copied and stored within the internal queues of all connected data sinks, from where it can be retrieved, i.e.
```C++
/* DataConsumerThread_1.inot */
Serial.println(counter);
```
```C++
/* DataConsumerThread_2.inot */
Serial.println(counter);
```
If a thread tries to read from an empty `Sink` the thread is suspended and the next ready thread is scheduled. When a new value is written to a `Source` and consequently copied to a `Sink` the suspended thread is resumed and continuous execution (i.e. read the data and act upon it).

Since data is actually copied multiple threads can read data from a single source without data being lost. This is an advantage compared to a simple shared variable. Furthermore you can not write to `Sink` or read from a `Source` - attempting to do so results in a compilation error.

## Comparison
|  | :+1: | :-1: |
|:---:|:---:|:---:|
| `Shared` | :+1: Needs only to declared once (`SharedVariables.h`). | :-1: Basically a global variable, with all the disadvantages those entail.<br/> :-1: Size of internal queue fixed for ALL shared variables.<br/> :-1: No protection against misuse (i.e. reading from multiple threads).<br/> |
| `Sink`/`Source` | :+1: Define internal queue size separately for each `Sink`.<br/> :+1: Supports multiple data consumers for a single data producer.<br/> :+1: Read/Write protection - Can't read from `Source`, can't write to `Sink`.<br/> :+1: Mandatory connecting (plumbing) within main `*.ino`-file makes data flows easily visible.<br/> | :-1: Needs manual connection (plumbing) to connect `Sink`'s to `Source`'s. |
