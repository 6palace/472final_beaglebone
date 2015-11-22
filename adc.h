//adc.h

#include <stdio.h>
#include <time.h>
#include <unistd.h>

//Reads an ADC (4-7 for the phytec board) and returns its value
int readADC(int whichADC);