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

  Serial.prefix([](String const & /* msg */) -> String
                {
                  return String("$");
                });
  Serial.suffix([](String const & prefix, String const & msg) -> String
                {
                  /* NMEA checksum is calculated over the complete message
                   * starting with '$' and ending with the end of the message.
                   */
                  byte checksum = 0;
                  std::for_each(msg.c_str(),
                                msg.c_str() + msg.length(),
                                [&checksum](char const c)
                                {
                                  checksum ^= static_cast<uint8_t>(c);
                                });
                  /* Assemble the footer of the NMEA message. */
                  char footer[16] = {0};
                  snprintf(footer, sizeof(footer), "*%02X\r\n", checksum);
                  return String(footer);
                });

  for(;;)
  {
    /* Sleep between 5 and 500 ms */
    rtos::ThisThread::sleep_for(rtos::Kernel::Clock::duration_u32(random(5,500)));
    /* Print a fake NMEA GPRMC message:
     * $GPRMC,062101.714,A,5001.869,N,01912.114,E,955535.7,116.2,290520,000.0,W*45\r\n
     */
    Serial.block();

    Serial.print("GPRMC,");
    Serial.print(millis());
    Serial.print(",A,");
    Serial.print("5001.869,");
    Serial.print("N,");
    Serial.print("01912.114,");
    Serial.print("E,");
    Serial.print("955535.7,");
    Serial.print("116.2,");
    Serial.print("290520,");
    Serial.print("000.0,");
    Serial.print("W");

    Serial.unblock();
  }
}
