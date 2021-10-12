void setup()
{
  Serial.begin(115200);
  while (!Serial) { }

  Producer.start();
  Consumer.start();
}

void loop()
{
  rtos::ThisThread::yield();
}
