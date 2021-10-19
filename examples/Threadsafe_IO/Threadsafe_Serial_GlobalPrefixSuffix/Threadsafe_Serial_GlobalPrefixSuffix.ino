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

  Serial.global_prefix(serial_log_message_prefix);
  Serial.global_suffix(serial_log_message_suffix);

  Thread_1.start();
  Thread_2.start();
  Thread_3.start();
}

void loop()
{
  Serial.block();
  Serial.println("Thread #0: Lorem ipsum ...");
  Serial.unblock();

  /* If we don't hand back control then the main thread
   * will hog the CPU and all other thread's won't get
   * time to be executed.
   */
  rtos::ThisThread::yield();
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
