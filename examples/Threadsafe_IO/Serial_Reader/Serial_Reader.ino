/* This example demonstrates how multiple threads can subscribe to
 * reading from the same physical Serial interface. Incoming data
 * is copied into per-thread receive buffers so that no thread
 * can "steal" data from another thread by reading it first.
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
}
