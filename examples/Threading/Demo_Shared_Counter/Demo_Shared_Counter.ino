void setup()
{
  Producer.start();
  Consumer.start();
}

void loop()
{
  rtos::ThisThread::yield();
}
