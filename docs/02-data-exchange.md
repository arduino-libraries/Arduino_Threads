<img src="https://content.arduino.cc/website/Arduino_logo_teal.svg" height="100" align="right"/>

Data Exchange between Threads
=============================
## Introduction
When an Arduino sketch formerly consisting of just a single `loop()` is split into multiple `loop()` functions contained in multiple `*.inot` files it becomes necessary to define a thread-safe mechanism to exchange data between those threads. For example, one  `*.inot` file could calculate a math formula and send the result back to the main `*.ino` file to act upon it. Meanwhile the main `*.ino` file can take care of other tasks.

 `Arduino_Threads` supports two different mechanisms for data exchange between threads: `Shared` variables and `Sink`/`Source` semantics. Both have their pros and cons as described below.

## `Shared`
A `Shared` variable is a global variable accessible to all threads. It can be declared within a header file named `SharedVariables.h` which is automatically included at the top of each `*.inot`-file <!-- TODO: What if I create an .inot file manually? I'd need to add the include myself I suppose. -->. Shared variables are declared using the `SHARED` macro in the following way <!-- TODO: Would it be possible to adjust the macro like this: SHARED int counter; ? I guess not. I would at least reverse the order so that it reflects the usual order of declaring variables: type name. I'm actually not sure if promoting this macro makes it much easier to understand. I think "Shared<type> name" is just fine. -->:
```C++
/* SharedVariables.h */
SHARED(counter, int); /* A globally available, threadsafe, shared variable of type 'int'. */
```
<!-- TODO: Does it work with object types too? how would I do that? --><!-- TODO: "counter" might be a bit counter intuitive to represent a queue. How about "sensorReadings"? --> A shared variable semantically represents a FIFO ([First-In/First-Out](https://en.wikipedia.org/wiki/FIFO_(computing_and_electronics))) queue and can therefore contain multiple values <!-- TODO: Even though a queue is a well known concept I recommend adding a graphic here to make it crystal clear to anyone not familiar with it -->. The limit for the amount of queue entries can be specified and is 16 by default. If you need to specify a higher limit (e.g. 32 entries) you can define the shared variable like this: `Shared<int, 32> sensorReadings` . New values can be inserted into the queue by naturally using the assignment operator `=` the same way you do with any ordinary [plain-old-data](https://en.wikipedia.org/wiki/Passive_data_structure) (POD) type, i.e. `int`, `char`, ... <!-- TODO: Hmm, I get where we're coming from but IMO overloading the assignment operator for an Add operation can be misleading. It's not obvious what exactly happens when you use the assignment operator and therefore it may be more difficult to remeber and understand in the first place. I'd recommend sticking to a .add() or .push() function. --> 

```C++
/* Thread_1.inot */
counter = 10; /* Store a value into the shared variable in a threadsafe manner. */
```
If the internal queue is full the oldest element is discarded and the latest element is inserted into the queue. <!-- TODO: Is there a way to check if the queue is full? Is there a way to resize the queue at runtime? -->

Retrieving stored data works also very naturally like it would for any POD data type:
```C++
/* Thread_2.inot */
Serial.println(counter); /* Retrieves a value from the shared variable in a threadsafe manner. */
```
<!-- TODO: Aha, I see, the idea is to treat a shared variable just like a simple variable and hide the fact that it's a queue. It feels a bit odd to me it being FIFO because that means when reading from it I don't get the "latest" value unless I drain the queue in a while loop (which would block when all entries have been read). What if I'm interested in all the values in the queue (use it as a buffer)? -->Should the internal queue be empty when trying to read the latest available value then the thread reading the shared variable will be suspended and the next available thread will be schedulded<!-- TODO: Can this be prevented? Can I check the queue size? -->. Once a new value is stored inside the shared variable the suspended thread resumes operation and consumes the value which has been stored in the internal queue.<!-- TODO: I understand the paradigm but I have the feeling that this is not always the desired behaviour. I'd prefer something like values.await() -->

Since shared variables are globally accessible from every thread, each thread can read from or write to the shared variable. The user is responsible for using the shared variable in a responsible and sensible way, i.e. reading a shared variable from different threads is generally a bad idea, as on every read an item is removed from the queue within the shared variable and other threads can't access the read value anymore <!-- TODO: Hmm, but there is the peek function. Due to the lack of the usual queue function it may be a bit cumbersome to use though. -->.

## `Sink`/`Source`
The idea behind the `Sink`/`Source` semantics is to model data exchange between one data producer (`Source`) and zero, one or multiple data consumers (`Sink`). A data producer or `Source` can be declared in any `*.ino` or `*.inot`-file using the `SOURCE` macro <!-- TODO: Same comment as above. I'm not sure if the macro is easier to understand than the variable declaration itself. -->:
```C++
/* DataProducerThread.inot */
SOURCE(counter, int); /* Declaration of a data source of type `int`. */
```
In a similar way, a data consumer can be declared in any `*.ino` or `*.inot`-file using the `SINK` macro. In difference to `Shared` where the size of the internal queue is globally set for all shared variables <!-- TODO: Is that true? There is one mailbox per object and there is a template param to specify the size. --> you can define your desired internal buffer size separately for each `Sink`.
```C++
/* DataConsumerThread_1.inot */
SINK(counter, int); /* Declaration of a data sink of type `int` with a internal queue size of '1'. */
```
```C++
/* DataConsumerThread_2.inot */
SINK(counter, int, 10); /* Declaration of a data sink of type `int` with a internal queue size of '10'. */
```
In order to actually facilitate the flow of data from a source to a sink the sinks must be connected to the desired data source <!-- TODO: Can we make it work both ways? sink.connectTo() would just invoke the source.connectTo() -->. This is done within the main `ino`-file:
```C++
/* MySinkSourceDemo.ino */
DataProducerThread.counter.connectTo(DataConsumerThread_1.counter);
DataProducerThread.counter.connectTo(DataConsumerThread_2.counter);
```
Whenever a new value is assigned to a data source <!-- TODO: Same comment as above. The assignment operator to me is difficult to grasp for this use case -->, i.e.
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
If a thread tries to read from an empty `Sink` the thread is suspended and the next ready thread is scheduled. When a new value is written to a `Source` and consequently copied to a `Sink` the suspended thread is resumed and continuous execution (i.e. read the data and act upon it). <!-- TODO: See comments for "Shared" -->

Since the data added to the source is copied multiple threads can read data from a single source without data being lost. This is an advantage compared to a simple shared variable. Furthermore you cannot accidentally write to a `Sink` or read from a `Source`. Attempting to do so results in a compilation error.

## Comparison
|  | :+1: | :-1: |
|:---:|:---:|:---:|
| `Shared` | :+1: Needs to be declared only once (in `SharedVariables.h`). | :-1: Basically a global variable, with all the disadvantages those entail.<br/> :-1: Size of internal queue fixed for ALL shared variables.<!-- TODO: True?--><br/> :-1: No protection against misuse (i.e. reading from multiple threads).<br/> |
| `Sink`/`Source` | :+1: Define internal queue size separately for each `Sink`.<br/> :+1: Supports multiple data consumers for a single data producer.<br/> :+1: Read/Write protection: Can't read from `Source`, can't write to `Sink`.<br/> :+1: Mandatory connecting (plumbing) within main `*.ino`-file makes data flows easily visible.<br/> | :-1: Needs manual connection (plumbing) to connect `Sink`'s to `Source`'s. |
