void setup() {
  Wire.begin();
  Wire.setClock(100000);
}

void loop() {
  Wire.beginTransmission(0x60);
  Wire.write(0x0);
  Wire.endTransmission(false);
  Wire.requestFrom(0x60, 1);
  delay(random() % 100);
  ecc608_id = Wire.read();
}
