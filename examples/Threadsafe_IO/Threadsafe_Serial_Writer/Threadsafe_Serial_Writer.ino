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
}

void loop()
{
  Serial.block();
  Serial.print("[");
  Serial.print(millis());
  Serial.print("] Thread #0: Lorem ipsum ...");
  Serial.println();
  Serial.unblock();

  /* If we don't hand back control then the main thread
   * will hog the CPU and all other thread's won't get
   * time to be executed.
   */
  rtos::ThisThread::yield();
}
