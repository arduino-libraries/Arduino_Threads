void setup() {
  // put your setup code here, to run once:
  pinMode(LEDR, OUTPUT);
  ledblue.start();
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LEDR, HIGH);
  delay(1000);
  digitalWrite(LEDR, LOW);
  delay(1000);
}
