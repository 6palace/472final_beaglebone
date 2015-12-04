//motor.h
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

//property struct for pwm
typedef struct pwmAttributes {
	FILE* period;
	FILE* duty_cycle;
	FILE* enable;
} pwmAttr;

//gpio device operations
void setGPIO(FILE* loc, int toSet);
FILE* initGPIO(int gpioNum);

pwmAttr initPWM(int chipNum, int subID);

//updates pwm duty attributes depending on given percent
void motorSpeed(pwmAttr pwm, int percent);
//controls specified motor
void setMotor(int whichMotor, int direction, int percent);