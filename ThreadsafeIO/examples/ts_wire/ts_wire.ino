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

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

byte lsm6dsox_read_reg(byte const reg_addr);

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

WireBusDevice lsm6dsox{"Wire", WireBusDeviceConfig{LSM6DSOX_ADDRESS}};

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup()
{
  Serial.begin(9600);
  while (!Serial) { }

  Wire.begin();

  Serial.print("LSM6DSOX[WHO_AM_I] = 0x");
  Serial.println(lsm6dsox_read_reg(LSM6DSOX_WHO_AM_I_REG), HEX);
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
  
  IoRequest  req(&write_buf, sizeof(write_buf), &read_buf, sizeof(read_buf));
  IoResponse rsp = lsm6dsox.transfer(req);

  /* Optionally dp other stuff */

  rsp->wait();
  return rsp->read_buf[0];

  /*
  Wire.beginTransmission(LSM6DSOX_ADDRESS);
  Wire.write(reg_addr);
  Wire.endTransmission(false);

  Wire.requestFrom(LSM6DSOX_ADDRESS, 1);
  while(!Wire.available()) { }
  return Wire.read();
  */
}
