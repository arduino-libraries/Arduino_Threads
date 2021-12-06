/* This example demonstrates how multiple threads can write to
 * the same physical Serial interface without interfering with
 * one another.
 */

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
}
