//adc.h

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>

#define TIMER_TAG 263 //Unique number for timer signal identifier

//location of adc pipe
FILE* adcOut;

//Reads an ADC (4-7 for the phytec board) and returns its value
static int readADC(int whichADC);

//gets called on each timer expiration, writes to adc pipe
static void timeHandler(int sigNo, siginfo_t * evp, void * ucontext);