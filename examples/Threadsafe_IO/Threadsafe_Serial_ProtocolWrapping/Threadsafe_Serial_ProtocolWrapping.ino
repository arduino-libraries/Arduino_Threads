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

  GPS_Thread.start();
}

void loop()
{
  /* If we don't hand back control then the main thread
   * will hog the CPU and all other thread's won't get
   * time to be executed.
   */
  rtos::ThisThread::yield();
}
