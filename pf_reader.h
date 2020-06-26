void setup() {
  Wire.begin();
  Wire.setClock(400000);
}

void loop() {
  Wire.beginTransmission(0x8);
  Wire.write(0x0);
  Wire.endTransmission(false);
  Wire.requestFrom(0x8, 1);
  delay(random() % 100);
  pf1550_id = Wire.read();
}
