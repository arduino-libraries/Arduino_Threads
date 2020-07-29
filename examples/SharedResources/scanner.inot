void setup() {
  Wire.begin();
  Wire.setClock(100000);
}

void loop() {
  struct i2cScanResults results = {};
  for (int i = 0; i < 128; i++) {
    Wire.beginTransmission (i);
    results.address[i] = (Wire.endTransmission () == 0);
  }
  scanResults = results;
  delay(random() % 1000);
}
