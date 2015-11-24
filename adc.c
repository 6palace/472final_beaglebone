//adc.c

#include "adc.h"      
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main() {

	mknod("/tmp/adcData4", S_IFIFO, 0);
	mknod("/tmp/adcData5", S_IFIFO, 0);
	mknod("/tmp/adcData6", S_IFIFO, 0);
	mknod("/tmp/adcData7", S_IFIFO, 0);

	FILE* adc4 = fopen("/tmp/adcData4", "w");
	//fprintf(adc4, "ADC Value 4: %d\n", readADC(4));
	while(1) {

		//fprintf(adc4, "ADC Value 4: %d\n", readADC(4));
		fprintf(adc4, "Hi\n");
		//fprintf(adc5, "ADC Value 5: %d\n", readADC(5));
		//fprintf(adc6, "ADC Value 6: %d\n", readADC(6));
		//fprintf(adc7, "ADC Value 7: %d\n", readADC(7));
		//printf("sent\n");
		//usleep(1000000);
		fflush(adc4);
		sleep(1);

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