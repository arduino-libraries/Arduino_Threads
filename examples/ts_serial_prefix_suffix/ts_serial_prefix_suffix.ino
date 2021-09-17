/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_ThreadsafeIO.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static size_t constexpr NUM_THREADS = 5;

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

void serial_thread_func();

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

void serial_thread_func()
{
  Serial.begin(9600);

  char const * thread_name = rtos::ThisThread::get_name();
  Serial.prefix([thread_name]() -> String
                {
                  char msg[64] = {0};
                  snprintf(msg, sizeof(msg), "[%05lu] %s ", millis(), thread_name);
                  return String(msg);
                });
  Serial.suffix([]() -> String
                {
                  return String("\r\n");
                });

  for(;;)
  {
    /* Sleep between 5 and 500 ms */
    rtos::ThisThread::sleep_for(rtos::Kernel::Clock::duration_u32(random(5,500)));
    /* Print a unbroken log message including thread name and timestamp as a prefix. */
    Serial.block();
    Serial.print("My ");
    Serial.print("unbroken ");
    Serial.print("thread-safe ");
    Serial.print("message!");
    Serial.unblock();
  }
}
