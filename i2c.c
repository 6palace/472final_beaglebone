#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#define ACCEL_XOUT_H       3B    
#define ACCEL_XOUT_L       3C    
#define ACCEL_YOUT_H       3D   
#define ACCEL_YOUT_L       3E   
#define ACCEL_ZOUT_H       3F    
#define ACCEL_ZOUT_L       40    
#define TEMP_OUT_H         41    
#define TEMP_OUT_L         42    
#define GYRO_XOUT_H        43    
#define GYRO_XOUT_L        44   
#define GYRO_YOUT_H        45   
#define GYRO_YOUT_L        46    
#define GYRO_ZOUT_H        47    
#define GYRO_ZOUT_L        48 

static u_int16_t rawDataCollect(char* high, char* low, int i2cHandle);  

int main(void)
{
  int gyroAddress = 0x68; // gyro device address
  int tenBitAddress = 0;  // is the device's address 10-bit? Usually not.
  int res = 0;   // for error checking of operations
  u_int16_t data_raw[7];
  double data_convert[7];

  //char high_values[] = {ACCEL_XOUT_H, ACCEL_YOUT_H, ACCEL_ZOUT_H, TEMP_OUT_H, GYRO_XOUT_H, GYRO_YOUT_H, GYRO_ZOUT_H};
  //char low_values[] = {ACCEL_XOUT_L, ACCEL_YOUT_L, ACCEL_ZOUT_L, TEMP_OUT_L, GYRO_XOUT_L, GYRO_YOUT_L, GYRO_ZOUT_L};
  
  // Create a file descriptor for the I2C bus
  int i2cHandle = open("/dev/i2c-2", O_RDWR);

  // Tell the I2C peripheral that the device address is (or isn't) a 10-bit
  //   value. Most probably won't be.
  res = ioctl(i2cHandle, I2C_TENBIT, tenBitAddress);

  // Tell the I2C peripheral what the address of the device is. We're going to
  //   start out by talking to the gyro.
  res = ioctl(i2cHandle, I2C_SLAVE, gyroAddress);



  memset(data_raw, 0, sizeof(data_raw));
  memset(data_convert, 0, sizeof(data_convert));


  for(int i = 0; i < 7; i++) {
    data_raw[i] = rawDataCollect("0x41", "0x42", i2cHandle);
    printf("value : %d\n", i2cHandle);
  }



  data_convert[3] = (double) data_raw[3];
  data_convert[3] = (data_convert[3] / 340.0) + 36.53;

  printf("Temperature: %f\n", data_convert[3]);
  printf("Acceleration - x:axis: %f\n", data_convert[0]);
}


u_int16_t rawDataCollect(char* high, char* low, int i2cHandle) {
  printf("High: %s\n", high);
  printf("Low: %s\n", low);
  char txBuffer[32]; 
  char rxBuffer[32]; 
  memset(txBuffer, 0, sizeof(txBuffer));
  memset(rxBuffer, 0, sizeof(rxBuffer));
  u_int16_t concat_data = 0;
  txBuffer[0] = 0x41; // This is the address we want to read from.
  printf("txBuffer value: %d\n", txBuffer[0]);
  write(i2cHandle, txBuffer, 1);
  read(i2cHandle, rxBuffer, 1);
  printf("rxBuffer data: %s\n", rxBuffer);
  concat_data = (u_int16_t)* rxBuffer;
  printf("concat_data value: %d\n", concat_data);

  txBuffer[0] = 0x42; // This is the address we want to read from.
  write(i2cHandle, txBuffer, 1);
  read(i2cHandle, rxBuffer, 1);
  concat_data = (concat_data << 8);
  concat_data |= (u_int16_t)* rxBuffer;

  return concat_data;
}