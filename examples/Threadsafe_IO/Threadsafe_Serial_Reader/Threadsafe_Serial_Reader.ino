/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_Threads.h>

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(9600);
  while (!Serial) { }

  Thread_1.start();
  Thread_2.start();
  Thread_3.start();

  Serial.block();
  Serial.println("Thread #0 started.");
  Serial.unblock();
}

void loop()
{
  /* Read data from the serial interface into a String. */
  String serial_msg;
  while (Serial.available())
    serial_msg += (char)Serial.read();

  /* Print thread id and chip id value to serial. */
  if (serial_msg.length())
  {
    Serial.block();
    Serial.print("[");
    Serial.print(millis());
    Serial.print("] Thread #0: ");
    Serial.print(serial_msg);
    Serial.println();
    Serial.unblock();
  }

  /* If we don't hand back control then the main thread
   * will hog the CPU and all other thread's won't get
   * time to be executed.
   */
  rtos::ThisThread::yield();
}
