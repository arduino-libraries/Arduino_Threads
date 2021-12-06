/* This examples demonstrates the SOURCE/SINK abstraction. Each thread
 * may have any number of SOURCES and SINKS that can be connected
 * together using the 'connectTo' method.
 */

void setup()
{
  Producer.counter.connectTo(Consumer.counter);
  Producer.start();
  Consumer.start();
}

void loop()
{

}
