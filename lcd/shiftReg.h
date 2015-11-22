//EE 472 Group F
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h> 

//using three gpios, initiate the shift register inputs
void initSR(void);
//method to create gpio char driver
void initGPIO(int gpioNum);
//sends byte of data into shift register and then latch to output in parallel
void sendByte(char byte);
