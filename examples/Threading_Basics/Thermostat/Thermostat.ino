/* This example emulates a thermostat system which consists of
 * a single temperature sensore and multiple heating devices
 * or air-conditioners. The temperature sensor provides periodic
 * temperature sensor measurements and acts as a temperature source.
 * This temperature is consumed by various TemperatureControl_ threads
 * which perform the act of actual room temperature control.
 * 
 * Note: While there is only a single temperature "source" there are
 * multiple temperature "sinks". The source/sink paradigm is constructed
 * in such a way, that each sink is guaranteed to see every value provided
 * by a source. This is something that can not be modeled using the shared
 * variable abstraction.
 */

void setup()
{
  /* Connect the temperature sensor thread providing temperature readings
   * with the various temperature control unit threads.
   */
  CONNECT(TemperatureSensor, temperature, HeatingControl, temperature);
  CONNECT(TemperatureSensor, temperature, AirConditionerControl, temperature);
  CONNECT(TemperatureSensor, temperature, TemperatureSensorReporter, temperature);

  /* Start the individual threads for sensing the temperature
   * and controlling heating/air-conditioning based on the sensed
   * temperature on a per-room basis.
   */
  TemperatureSensor.start();
  HeatingControl.start();
  AirConditionerControl.start();
  TemperatureSensorReporter.start();
}

void loop()
{

}
