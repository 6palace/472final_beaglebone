//motor.h
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef struct pwmAttributes {
	FILE* period;
	FILE* duty_cycle;
	FILE* enable;
} pwmAttr;


void setGPIO(FILE* loc, int toSet);
FILE* initGPIO(int gpioNum);
pwmAttr initPWM(int chipNum, int subID);
void motorSpeed(pwmAttr pwm, int percent);