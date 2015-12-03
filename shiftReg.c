
#include "shiftReg.h"

#define SERIAL_SR_DATA 31
#define SERIAL_SR_LATCH 30
#define SERIAL_SR_CLK 60
#define DELAY 1

void initSR() {
	initGPIO(SERIAL_SR_DATA);
	initGPIO(SERIAL_SR_LATCH);
	initGPIO(SERIAL_SR_CLK);
}

void sendByte(char byte) {
	gpio_set_value(SERIAL_SR_LATCH, 0);
	for (int i = 7; i >= 0; i--) {
		gpio_set_value(SERIAL_SR_CLK, 0);   
		gpio_set_value(SERIAL_SR_DATA, (byte>>i) & 1); 
		udelay(DELAY);
		gpio_set_value(SERIAL_SR_CLK, 1);  
		udelay(DELAY);
	}
	gpio_set_value(SERIAL_SR_LATCH, 1);
	udelay(DELAY);
	gpio_set_value(SERIAL_SR_LATCH, 0);

}

void initGPIO(int gpioNum) {
   char labstr[] = "gpio";
   gpio_request(gpioNum, labstr);
   gpio_direction_output(gpioNum, 1);
   gpio_set_value(gpioNum, 0);
}
