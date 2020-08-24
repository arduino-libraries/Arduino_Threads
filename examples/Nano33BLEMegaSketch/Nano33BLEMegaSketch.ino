void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  Serial.tags();
  Humidity.start();
  Gesture.start();
  Accelerometer.start();
}

void loop() {
  // put your main code here, to run repeatedly:

}
