void setup() {
  Wire.begin();
  Wire.setClock(100000);
}

void loop() {
  Wire.beginTransmission(0x60);
  Wire.write((uint8_t)0x0);
  Wire.endTransmission(false);
  int res = Wire.requestFrom(0x60, 1);
  delay(random() % 5);
  if (res == 1) {
    ecc608_id = Wire.read();
  }
}
