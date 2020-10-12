/*
 * This examples demonstrates the SOURCE/SINK abstraction.
 * Each thread may have any number of SOURCES and SINKS that can be connected
 * together using the "connectTo" method.
 */

void setup() {
  data_reader.out.connectTo(data_writer.in);
  data_reader.start();
  data_writer.start();

  // put your setup code here, to run once:
  pinMode(LEDR, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LEDR, HIGH);
  delay(1000);
  digitalWrite(LEDR, LOW);
  delay(1000);
}
