#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#define ACCEL_XOUT_H       0x3B    
#define ACCEL_XOUT_L       0x3C    
#define ACCEL_YOUT_H       0x3D   
#define ACCEL_YOUT_L       0x3E   
#define ACCEL_ZOUT_H       0x3F    
#define ACCEL_ZOUT_L       0x40    
#define TEMP_OUT_H         0x41    
#define TEMP_OUT_L         0x42    
#define GYRO_XOUT_H        0x43    
#define GYRO_XOUT_L        0x44   
#define GYRO_YOUT_H        0x45   
#define GYRO_YOUT_L        0x46    
#define GYRO_ZOUT_H        0x47    
#define GYRO_ZOUT_L        0x48 

#define MPU6050_AFS_SEL0   MPU6050_D3
#define MPU6050_AFS_SEL1   MPU6050_D4

static u_int16_t rawDataCollect(char high, char low, int i2cHandle); 
static int16_t twoConvert(u_int16_t data);

int main(void)
{
  int gyroAddress = 0x68; // gyro device address
  int tenBitAddress = 0;  // is the device's address 10-bit? Usually not.
  int res = 0;   // for error checking of operations
  int16_t data_raw[7];
  double data_convert[7];
  u_int16_t  raw_temp = 0;
  u_int16_t  raw_accelX = 0;
  u_int16_t  raw_accelY = 0;
  u_int16_t  raw_accelZ = 0;

  char high_values[] = {ACCEL_XOUT_H, ACCEL_YOUT_H, ACCEL_ZOUT_H, TEMP_OUT_H, GYRO_XOUT_H, GYRO_YOUT_H, GYRO_ZOUT_H};
  char low_values[] = {ACCEL_XOUT_L, ACCEL_YOUT_L, ACCEL_ZOUT_L, TEMP_OUT_L, GYRO_XOUT_L, GYRO_YOUT_L, GYRO_ZOUT_L};
  
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

  
  raw_accelX = rawDataCollect(high_values[0], low_values[0], i2cHandle);
  raw_accelY = rawDataCollect(high_values[1], low_values[1], i2cHandle);
  raw_accelZ = rawDataCollect(high_values[2], low_values[2], i2cHandle);
  raw_temp = rawDataCollect(high_values[3], low_values[3], i2cHandle);

  // raw_accelX = twoConvert(raw_accelX);
  // raw_accelY = twoConvert(raw_accelY);
  // raw_accelZ = twoConvert(raw_accelZ);

  printf("Temperature: %f Celsius\n", (twoConvert(raw_temp)/340.0 + 32.25));
  printf("Acceleration - X:axis: %d\n", raw_accelX);
  printf("Acceleration - Y:axis: %d\n", raw_accelY);
  printf("Acceleration - Z:axis: %d\n", raw_accelZ);
}


u_int16_t rawDataCollect(char high, char low, int i2cHandle) {
  printf("High: %c\n", high);
  printf("Low: %c\n", low);
  char txBuffer[32]; 
  char rxBuffer[32]; 
  u_int16_t concat_data = 0;

  memset(txBuffer, 0, sizeof(txBuffer));
  memset(rxBuffer, 0, sizeof(rxBuffer));

 
  txBuffer[0] = high; // This is the address we want to read from.
  printf("txBuffer value: %d\n", txBuffer[0]);
  write(i2cHandle, txBuffer, 1);
  read(i2cHandle, rxBuffer, 1);
  concat_data = (u_int16_t)* rxBuffer;

  txBuffer[0] = low; // This is the address we want to read from.
  write(i2cHandle, txBuffer, 1);
  read(i2cHandle, rxBuffer, 1);
  concat_data = (concat_data << 8 | (u_int16_t)* rxBuffer);

  return concat_data;
}

int16_t twoConvert(u_int16_t data) {
  int neg = ((data >> 15) & 1 );
  if(neg) {
    int mask = 0xffff;
    data = -1 * (data ^ mask);
  }
  return data;
}