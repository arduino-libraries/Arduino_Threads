/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <ThreadsafeIO.h>

#include <Wire.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static byte constexpr LSM6DSOX_ADDRESS      = 0x6A;
static byte constexpr LSM6DSOX_WHO_AM_I_REG = 0x0F;

static size_t constexpr NUM_THREADS = 20;

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

byte lsm6dsox_read_reg(byte const reg_addr);
void lsm6dsox_thread_func();

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

WireBusDevice lsm6dsox{"Wire", WireBusDeviceConfig{LSM6DSOX_ADDRESS}};

static char thread_name[NUM_THREADS][32];

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(9600);
  while (!Serial) { }

  /* Fire up some threads all accessing the LSM6DSOX */
  for(size_t i = 0; i < NUM_THREADS; i++)
  {
    snprintf(thread_name[i], sizeof(thread_name[i]), "Thread #%02d", i);
    rtos::Thread * t = new rtos::Thread(osPriorityNormal, OS_STACK_SIZE, nullptr, thread_name[i]);
    t->start(lsm6dsox_thread_func);
  }
}

void loop()
{

}

/**************************************************************************************
 * FUNCTION DEFINITION
 **************************************************************************************/

byte lsm6dsox_read_reg(byte const reg_addr)
{
  /* As we need only 1 byte large write/read buffers for this IO transaction
   * the buffers are not arrays but rather simple variables.
   */
  byte write_buf = reg_addr;
  byte read_buf  = 0;
  
  IoRequest  req(write_buf, read_buf);
  IoResponse rsp = lsm6dsox.transfer(req);
  
  /* Optionally do other stuff */

  rsp->wait();
  
  return read_buf;
}


void lsm6dsox_thread_func()
{
  for(;;)
  {
    /* Sleep between 5 and 500 ms */
    rtos::ThisThread::sleep_for(rtos::Kernel::Clock::duration_u32(random(5,500)));
    /* Try to read some data from the BMP3888. */
    byte const who_am_i = lsm6dsox_read_reg(LSM6DSOX_WHO_AM_I_REG);
    /* Print thread id and chip id value to serial. */
    char msg[64] = {0};
    snprintf(msg, sizeof(msg), "%s: LSM6DSOX[WHO_AM_I] = 0x%X", rtos::ThisThread::get_name(), who_am_i);
    Serial.println(msg);
  }
}
