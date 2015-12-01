//motor.c

#include "motor.h"



//PINS!
#define GPIO_AIN1 71
#define GPIO_AIN2 73
#define GPIO_BIN1 72
#define GPIO_BIN2 70
#define CHIP_PWMA 0
#define ID_PWMA 0
#define CHIP_PWMB 4
#define ID_PWMB 0

//Directions
#define OFF 0
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3

//MOTORS
#define M_RIGHT 0
#define M_LEFT 1


FILE* ain1;
FILE* ain2;
FILE* bin1;
FILE* bin2;
pwmAttr pwmA;
pwmAttr pwmB;

int main() {
	ain1 = initGPIO(GPIO_AIN1);
	ain2 = initGPIO(GPIO_AIN2);
	bin1 = initGPIO(GPIO_BIN1);
	bin2 = initGPIO(GPIO_BIN2);
	pwmA = initPWM(CHIP_PWMA, ID_PWMA);
	pwmB = initPWM(CHIP_PWMB, ID_PWMB);
	//fprintf(pwmA.enable, "%s", "1");
	//fprintf(pwmB.enable, "%s", "1");

	setMotor(M_RIGHT, FORWARD, 100);
	sleep(1);
	setMotor(M_RIGHT, OFF, 100);
	sleep(1);
	setMotor(M_LEFT, FORWARD, 100);
	sleep(1);
	setMotor(M_LEFT, OFF, 100);

	return 0;
}

void setMotor(int whichMotor, int direction, int percent) {
	
	FILE* in1;
	FILE* in2;
	pwmAttr pwm;

	if(whichMotor == M_RIGHT) {
		in1 = ain1;
		in2 = ain2;
		pwm = pwmA;
		printf("RIGHT!\n");
	} else {		
		in1 = bin1;
		in2 = bin2;
		pwm = pwmB;
		printf("LEFT!\n");
	}

	switch(direction){
		case OFF:
			setGPIO(in1, 0);
			setGPIO(in2, 0);
			break;
		case FORWARD:
			setGPIO(in1, 0);
			setGPIO(in2, 1);
			break;
		case BACKWARD:
			setGPIO(in1, 1);
			setGPIO(in2, 0);
			break;
		case BRAKE:
			setGPIO(in1, 1);
			setGPIO(in2, 1);
			break;
	}

	motorSpeed(pwm, percent);
}


void motorSpeed(pwmAttr pwm, int percent) {
	int period = 100;
	//fprintf(pwm.period, "%d", period);
	//fprintf(pwm.duty_cycle, "%d", period*(percent/100));
	//fflush(pwm.period);
	//fflush(pwm.duty_cycle);
}

//Sets a GPIO pin to the specified value
void setGPIO(FILE* loc, int toSet) {
	//fprintf(loc, "%d", toSet);
	//fflush(loc);
}

//Takes a GPIO pin number and returns a handle to a file where the GPIO pin can be controlled
FILE* initGPIO(int gpioNum) {
	/*FILE *dir, *sys, *gpio;

	sys = fopen("/sys/class/gpio/export", "w");
	fseek(sys, 0, SEEK_SET);
	fprintf(sys, "%d", gpioNum);
	fflush(sys);
	fclose(sys);

	char directionLoc[256];
	sprintf (directionLoc, "/sys/class/gpio/gpio%d/direction", gpioNum);
	dir = fopen(directionLoc, "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "out");
	fflush(dir);
	fclose(dir);

	char valueLoc[256];
	sprintf (valueLoc, "/sys/class/gpio/gpio%d/value", gpioNum);
	gpio = fopen(valueLoc, "w");
	fseek(gpio, 0, SEEK_SET);*/
	return NULL;
}

//creates a pwmAttr and sets up member files for future access
pwmAttr initPWM(int chipNum, int subID) {
	pwmAttr attrib;

	/*char exportC[256];
	sprintf(exportC, "/sys/class/pwm/pwmchip%d/export", chipNum);
	FILE *PWM1 = fopen(exportC, "w");
	fprintf(PWM1, "%s", "0");
	fflush(PWM1);
	fclose(PWM1);

	char periodC[256];
	sprintf (periodC, "/sys/class/pwm/pwmchip%d/pwm%d/period", chipNum, subID);
	attrib.period = fopen(periodC, "w");

	char dutyC[256];
	sprintf (dutyC, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chipNum, subID);
	attrib.duty_cycle = fopen(dutyC, "w");

	//char enableC[256];
	//sprintf (enableC, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chipNum, subID);
	//attrib.enable = fopen(enableC, "w");*/

	return attrib;
}