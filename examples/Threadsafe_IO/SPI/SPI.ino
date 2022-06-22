/* This example demonstrates how multiple threads can communicate
 * with a single SPI client device using the BusDevice abstraction
 * for SPI. In a similar way multiple threads can interface
 * with different client devices on the same SPI bus.
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_Threads.h>

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static int  const BMP388_CS_PIN  = 2;
static int  const BMP388_INT_PIN = 6;
static byte const BMP388_CHIP_ID_REG_ADDR = 0x00;

static size_t constexpr NUM_THREADS = 20;

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

byte bmp388_read_reg(byte const reg_addr);
void bmp388_thread_func();

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

BusDevice bmp388(SPI, BMP388_CS_PIN, 1000000, MSBFIRST, SPI_MODE0);

static char thread_name[NUM_THREADS][32];

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(9600);
  while (!Serial) { }

  pinMode(BMP388_CS_PIN, OUTPUT);
  digitalWrite(BMP388_CS_PIN, HIGH);

  for(size_t i = 0; i < NUM_THREADS; i++)
  {
    snprintf(thread_name[i], sizeof(thread_name[i]), "Thread #%02d", i);
    rtos::Thread * t = new rtos::Thread(osPriorityNormal, OS_STACK_SIZE, nullptr, thread_name[i]);
    t->start(bmp388_thread_func);
  }
}

void loop()
{

}

/**************************************************************************************
 * FUNCTION DEFINITION
 **************************************************************************************/

byte bmp388_read_reg(byte const reg_addr)
{
  /* REG_ADDR | DUMMY_BYTE | REG_VAL is on SDO */
  byte read_write_buf[] = {static_cast<byte>(0x80 | reg_addr), 0, 0};

  IoRequest req(read_write_buf, sizeof(read_write_buf), nullptr, 0);
  IoResponse rsp = transferAndWait(bmp388, req);

  return read_write_buf[2];
}

void bmp388_thread_func()
{
  Serial.begin(9600);
  while(!Serial) { }

  for(;;)
  {
    /* Sleep between 5 and 500 ms */
    rtos::ThisThread::sleep_for(rtos::Kernel::Clock::duration_u32(random(5,500)));
    /* Try to read some data from the BMP3888. */
    byte const chip_id = bmp388_read_reg(BMP388_CHIP_ID_REG_ADDR);
    /* Print thread id and chip id value to serial. */
    char msg[64] = {0};
    snprintf(msg, sizeof(msg), "%s: Chip ID = 0x%X", rtos::ThisThread::get_name(), chip_id);
    Serial.println(msg);
  }
}
