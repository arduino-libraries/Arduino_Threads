/* This example demonstrates how every Serial message can be prefixed
 * as well as suffixed by a user-configurable message. In this example
 * this functionality is used for prepending the right header for a
 * pseudo NMEA-encoded (think GPS) message as well as for calculating
 * and appending the checksum at the end.
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

  GPS_Thread.start();
}

void loop()
{

}
