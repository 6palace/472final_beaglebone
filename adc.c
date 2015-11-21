//adc.c

#include "adc.h"

int main() {
	printf("ADC Value 4: %d\n", readADC(4));
	printf("ADC Value 5: %d\n", readADC(5));
	printf("ADC Value 6: %d\n", readADC(6));
	printf("ADC Value 7: %d\n", readADC(7));
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