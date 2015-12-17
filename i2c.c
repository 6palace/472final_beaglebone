
#include "i2c.h"

int main(void)
{
   struct sigevent    sigx;
   struct sigaction   sigAct;
   struct itimerspec  newTime;
   timer_t timer;

   sigemptyset(&sigAct.sa_mask);
   sigAct.sa_flags = 0;
   sigAct.sa_handler = (void (*))timeHandler;
   sigaction(SIGUSR1, &sigAct, 0);


   // printf("signal setup\n");

   sigx.sigev_notify          = SIGEV_SIGNAL;
   sigx.sigev_signo           = SIGUSR1;
   sigx.sigev_value.sival_int = TIMER_TAG;

   newTime.it_value.tv_sec     = 1; 
   newTime.it_value.tv_nsec    = 2;
   newTime.it_interval.tv_sec  = 2;
   newTime.it_interval.tv_nsec = 0;

   timer_create(CLOCK_REALTIME, &sigx, &timer);
   timer_settime(timer, 0,  &newTime, NULL);
   //printf("DONE");

   while(1)
      pause();

  return 0;
}


u_int16_t rawDataCollect(char high, char low, int i2cHandle) {
  //printf("High: %c\n", high);
  //printf("Low: %c\n", low);
  char txBuffer[32]; 
  char rxBuffer[32]; 
  u_int16_t concat_data = 0;

  memset(txBuffer, 0, sizeof(txBuffer));
  memset(rxBuffer, 0, sizeof(rxBuffer));

 
  txBuffer[0] = high; // This is the address we want to read from.
  //printf("txBuffer value: %d\n", txBuffer[0]);
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

static void timeHandler(int sigNo, siginfo_t * evp, void * ucontext) {
  FILE* i2cOut = fopen("/tmp/i2cData", "w");
  fprintf(i2cOut, "%f\n", convert_tempr());
  fclose(i2cOut);
}

static double convert_tempr(void) {

  int gyroAddress = 0x68; // gyro device address
  int tenBitAddress = 0;  // is the device's address 10-bit? Usually not.
  int res = 0;   // for error checking of operations
  int16_t data_raw[7];
  double data_convert[7];
  u_int16_t  raw_temp = 0;

  double  convert_temp = 0;

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

  raw_temp = rawDataCollect(high_values[3], low_values[3], i2cHandle);

  convert_temp = (twoConvert(raw_temp)/340.0 + 32.25);
  printf("Temperature: %f Celsius\n", convert_temp);
  
  return convert_temp;
}