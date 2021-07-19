/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <Arduino_ThreadsafeIO.h>

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

void bmp388_select();
void bmp388_deselect();
byte bmp388_read_reg(byte const reg_addr);
void bmp388_thread_func();

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

SpiBusDevice bmp388 = BusDeviceCreator.create("SPI", SPISettings{1000000, MSBFIRST, SPI_MODE0}, BMP388_CS_PIN);

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

void bmp388_select()
{
  digitalWrite(BMP388_CS_PIN, LOW);
}

void bmp388_deselect()
{
  digitalWrite(BMP388_CS_PIN, HIGH);
}

byte bmp388_read_reg(byte const reg_addr)
{
  byte const write_buf[3] =
  {
    static_cast<byte>(0x80 | reg_addr), /* REG_ADDR, if MSBit is set -> READ access */
    0,                                  /* Dummy byte.                              */
    0                                   /* REG_VAL is output on SDO                 */
  };
  byte read_buf[3] = {0};

  IoRequest req(write_buf, sizeof(write_buf), read_buf, sizeof(read_buf));
  IoResponse rsp = bmp388.transfer(req);

  /* Do other stuff */

  rsp->wait();

  return read_buf[2];
}

void bmp388_thread_func()
{
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
