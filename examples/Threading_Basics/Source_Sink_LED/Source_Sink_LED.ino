/* This examples demonstrates the SOURCE/SINK abstraction. Each thread
 * may have any number of SOURCES and SINKS that can be connected
 * together using the 'connectTo' method.
 */

void setup()
{
  Source_Thread.led.connectTo(Sink_Thread.led);
  Sink_Thread.start();
  Source_Thread.start();
}

void loop()
{

}
