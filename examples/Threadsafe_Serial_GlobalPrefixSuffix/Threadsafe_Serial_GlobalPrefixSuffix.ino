/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_Threads.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static size_t constexpr NUM_THREADS = 5;

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

String serial_log_message_prefix(String const & /* msg */);
String serial_log_message_suffix(String const & prefix, String const & msg);
void   serial_thread_func();

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

static char thread_name[NUM_THREADS][32];
#undef Serial
#ifdef ARDUINO_PORTENTA_H7_M4
  SerialDispatcher Serial(Serial1); /* No SerialUSB for Portenta H7 / M4 Core */
#else
  SerialDispatcher Serial(SerialUSB);
#endif

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.global_prefix(serial_log_message_prefix);
  Serial.global_suffix(serial_log_message_suffix);

  /* Fire up some threads all accessing the LSM6DSOX */
  for(size_t i = 0; i < NUM_THREADS; i++)
  {
    snprintf(thread_name[i], sizeof(thread_name[i]), "Thread #%02d", i);
    rtos::Thread * t = new rtos::Thread(osPriorityNormal, OS_STACK_SIZE, nullptr, thread_name[i]);
    t->start(serial_thread_func);
  }
}

void loop()
{

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

void serial_thread_func()
{
  Serial.begin(9600);

  for(;;)
  {
    /* Sleep between 5 and 500 ms */
    rtos::ThisThread::sleep_for(rtos::Kernel::Clock::duration_u32(random(5,500)));
    /* Print a unbroken log message including thread name and timestamp as a prefix. */
    Serial.block();
    Serial.print(rtos::ThisThread::get_name());
    Serial.print(": ");
    Serial.print("Lorem ipsum ...");
    Serial.unblock();
  }
}
