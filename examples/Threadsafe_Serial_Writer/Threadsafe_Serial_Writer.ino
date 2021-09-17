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

  for(;;)
  {
    /* Sleep between 5 and 500 ms */
    rtos::ThisThread::sleep_for(rtos::Kernel::Clock::duration_u32(random(5,500)));
    /* Print thread id and chip id value to serial. */
    char msg[64] = {0};
    snprintf(msg, sizeof(msg), "[%05lu] %s: Lorem ipsum ...", millis(), rtos::ThisThread::get_name());
    /* Every Serial.print/println() encapsulated between
     * block/unblock statements will only be printed after
     * a block statement has occurred.
     */
    Serial.block();
    Serial.println(msg);
    Serial.unblock();
  }
}
