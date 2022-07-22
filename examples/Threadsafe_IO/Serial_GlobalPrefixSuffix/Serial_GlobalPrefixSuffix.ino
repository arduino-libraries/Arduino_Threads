/* This example demonstrates how every Serial message can be prefixed
 * as well as suffixed by a user-configurable message. In this example
 * this functionality is used for appending the current timestamp and
 * prepending a line feed. Other uses might be to prepend the thread
 * from which a given serial message is originating.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_Threads.h>
/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

String serial_log_message_prefix(String const & /* msg */);
String serial_log_message_suffix(String const & prefix, String const & msg);

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(9600);
  while (!Serial) { }

  Serial.globalPrefix(serial_log_message_prefix);
  Serial.globalSuffix(serial_log_message_suffix);

  Thread_1.start();
  Thread_2.start();
  Thread_3.start();
}

void loop()
{
  Serial.block();
  Serial.println("Thread #0: Lorem ipsum ...");
  Serial.unblock();
}

/**************************************************************************************
 * FUNCTION DEFINITION
 **************************************************************************************/

String serial_log_message_prefix(String const & /* msg */)
{
  char msg[32] = {0};
  snprintf(msg, sizeof(msg), "[%05lu] ", millis());
  return String(msg);
}

String serial_log_message_suffix(String const & prefix, String const & msg)
{
  return String("\r\n");
}
