
void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Insert tags before Serial transmission to distinguish 
  // the thread that originated them
  Serial.tags(true);

  // By declaring a serial "raw" we ask the dispatcher to flush
  // every N milliseconds without waiting for EOL (for this thread only)
  //Serial.raw();

  TempReader.begin();
  SerialReader.begin();
  GetRandom.begin();
  Accelerometer.begin();
}

void loop() {
  //Serial.println(temperature);          // blocks until new data is available
  Serial.println(temperature.peek());   // returns immediately the last known value

  if (Serial.available()) {
    Serial.println("Got something");
    while (Serial.available()) {
      Serial.write(Serial.read());
    }
  }

  Serial.println("ECC608 random number: " + String(randomNumber));
}
