//i2c.h
//EE472 Group F Lab 4


#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>


#define TIMER_TAG 264 //Unique number for timer signal identifier

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
static void timeHandler(int sigNo, siginfo_t * evp, void * ucontext);
static double convert_tempr(void); 
