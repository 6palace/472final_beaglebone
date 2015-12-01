//adc.c

#include "adc.h"      
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

int main() {
   
   srand(time(NULL));

	mknod("/tmp/adcData", S_IFIFO, 0);
	FILE* adcOut = fopen("/tmp/adcData", "w");
	while(1) {
		int x1 = readADC(4);
		int x2 = readADC(4);
		int x3 = readADC(4);
		int x4 = readADC(4);
		//fprintf(adcOut, "%d,%d,%d,%d\n", readADC(4), readADC(5), readADC(6), readADC(7));
		fprintf(adcOut, "%d,%d,%d,%d\n", x1, x2, x3, x4);
		printf("SENT: %d,%d,%d,%d\n", x1, x2, x3, x4);
		fflush(adcOut);
		usleep(1000000);
	}
	return 0;
}

int readADC(int whichADC) {
	/*FILE* gpio;
	int adcVal;
	char valueLoc[256];
	sprintf (valueLoc, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", whichADC);
	gpio = fopen(valueLoc, "r");
	fscanf(gpio, "%d", &adcVal);
	fclose(gpio);*/
	int adcVal = rand()/4;
	//int adcVal = 1023456789;
	return adcVal;
}