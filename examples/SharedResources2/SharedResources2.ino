void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Insert tags before Serial transmission to distinguish 
  // the thread that originated them
  Serial.tags(true);

  temp_reader_obj.start();
  pf_reader_obj.start();
  scanner_obj.start();
  ecc_reader_obj.start();
}

void loop() {
  //Serial.println(temperature);          // blocks until new data is available
  //Serial.println(temperature.peek());   // returns immediately the last known value

  struct i2cScanResults results = scanResults;
  for (int i = 0; i < 128; i++) {
    if (results.address[i] == true) {
      Serial.println("0x" + String(i, HEX));
    }
  }

  if (Serial.available()) {
    Serial.println("Got something");
    while (Serial.available()) {
      Serial.write(Serial.read());
    }
  }

  Serial.println("PF1550 ID: " + String(pf1550_id, HEX));
  Serial.println("ECC608 random number: " + String(randomNumber));
}
