//adc.c

#include "adc.h"      
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
	mknod("/tmp/adcData", S_IFIFO, 0);
	FILE* adcOut = fopen("/tmp/adcData", "w");
	while(1) {
		fprintf(adcOut, "%d,%d,%d,%d\n", readADC(4), readADC(5), readADC(6), readADC(7));
		fflush(adcOut);
		usleep(1000000);
	}
	return 0;
}


int readADC(int whichADC) {
	FILE* gpio;
	int adcVal;
	char valueLoc[256];
	sprintf (valueLoc, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", whichADC);
	gpio = fopen(valueLoc, "r");
	fscanf(gpio, "%d", &adcVal);
	fclose(gpio);
	return adcVal;
}