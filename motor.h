//motor.h
//EE472 Group F Lab 3

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
static void setGPIO(FILE* loc, int toSet);
static FILE* initGPIO(int gpioNum);

//sets up pwm in file system and links pwm attributes
static pwmAttr initPWM(int chipNum, int subID);

//updates pwm duty attributes depending on given percent
static void motorSpeed(pwmAttr pwm, int percent);
//controls specified motor
static void setMotor(int whichMotor, int direction, int percent);

