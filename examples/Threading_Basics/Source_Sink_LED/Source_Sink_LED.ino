/* This examples demonstrates the SOURCE/SINK abstraction. Each thread
 * may have any number of SOURCES and SINKS that can be connected
 * together using the 'connectTo' method.
 */

void setup()
{
  Source_ThreadPrivate::led.connectTo(Sink_ThreadPrivate::led);
  Sink_Thread.start();
  Source_Thread.start();
}

void loop()
{

}
