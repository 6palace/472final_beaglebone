//motor.c
//EE472 Group F Lab 3

#include "motor.h"

//PINS!
#define GPIO_AIN1 44
#define GPIO_AIN2 47
#define GPIO_BIN1 45
#define GPIO_BIN2 68
//#define CHIP_PWMA "48304100.ecap" //ecap moves
#define CHIP_PWMA "48302200.ehrpwm"
#define ID_PWMA 0
#define CHIP_PWMB "48302200.ehrpwm"
#define ID_PWMB 1

//Directions
#define OFF 0
#define FORWARD 1
#define BACKWARD 2
#define BRAKE 3

//MOTORS
#define M_RIGHT 0
#define M_LEFT 1

#define NUMPOLL 1



static int findPWM(char* whatPWM);

FILE* ain1;
FILE* ain2;
FILE* bin1;
FILE* bin2;
pwmAttr pwmA;
pwmAttr pwmB;


int main() {


   //Beagle init
   FILE* bbPinMux = fopen("/sys/devices/platform/ocp/ocp:P9_14_pinmux/state", "w");
   fprintf(bbPinMux, "pwm");
   fflush(bbPinMux);
   fclose(bbPinMux);   
   bbPinMux = fopen("/sys/devices/platform/ocp/ocp:P9_16_pinmux/state", "w");
   fprintf(bbPinMux, "pwm");
   fflush(bbPinMux);
   fclose(bbPinMux);

   printf("pwmchip is %d\n", findPWM(CHIP_PWMA));
   printf("pwmchip is %d\n", findPWM(CHIP_PWMB));

   //Beagle init done

   ain1 = initGPIO(GPIO_AIN1);
   ain2 = initGPIO(GPIO_AIN2);
   bin1 = initGPIO(GPIO_BIN1);
   bin2 = initGPIO(GPIO_BIN2);
   pwmA = initPWM(findPWM(CHIP_PWMA), ID_PWMA);
   pwmB = initPWM(findPWM(CHIP_PWMB), ID_PWMB);

   fprintf(pwmA.enable, "%s", "1");
   fprintf(pwmB.enable, "%s", "1");
   fclose(pwmA.enable);
   fclose(pwmB.enable);

   struct pollfd pfd[NUMPOLL];
   int ret;
   int fd;
   char* tok;

   fd = open("/tmp/motorData", O_RDONLY);
   pfd[0].fd = fd;   
   pfd[0].events = POLLIN;

   char databuf[1024] = "";

   while(1) {
      ret = poll(pfd, NUMPOLL, -1);
      if((ret > 0) && (pfd[0].revents & POLLIN)) { //Something happened
         read(fd, databuf, 1024);
         int argVal[3];
         // printf("Motor Get: %s\n", databuf);
         tok = strtok(databuf, "\n");
         while(tok != NULL) 
         {
           sscanf(tok, "%d,%d,%d", &argVal[0], &argVal[1], &argVal[2]);
           // printf("raw : %s", tok);
           printf("Motor Get: %d m %d m %d\n", argVal[0], argVal[1], argVal[2]);
           setMotor(argVal[0], argVal[1], argVal[2]);
           // printf(" %d\n", adcVals[i]);
           tok = strtok(NULL, "\n");
         }
      }
   }

   fclose(ain1);
   fclose(ain2);
   fclose(bin1);
   fclose(bin2);

   return 0;
}

static int findPWM(char* whatPWM) {
   printf("Finding %s\n", whatPWM);
   char pwmLoc[1024];
   sprintf (pwmLoc, "ls /sys/devices/platform/ocp/subsystem/devices/%s/pwm | grep -o '[0-9]'", whatPWM);
   int wherePWMint;
   char wherePWMstr[5];
   FILE* wherePWM = popen(pwmLoc, "r");
   fgets(wherePWMstr, sizeof(wherePWMstr), wherePWM);
   pclose(wherePWM);
   sscanf(wherePWMstr, "%d", &wherePWMint);
   return wherePWMint;
}

static void setMotor(int whichMotor, int direction, int percent) {

   FILE* in1;
   FILE* in2;
   pwmAttr pwm;

   if(whichMotor == M_RIGHT) {
      in1 = ain1;
      in2 = ain2;
      pwm = pwmA;
   } else {    
      in1 = bin1;
      in2 = bin2;
      pwm = pwmB;
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


static void motorSpeed(pwmAttr pwm, int percent) {
   int period = 10000; //10kHz
   fprintf(pwm.period, "%d", period);
   printf("%d\n", (period/100)*percent);
   fprintf(pwm.duty_cycle, "%d", (period/100)*percent);
   fflush(pwm.period);
   fflush(pwm.duty_cycle);
}

//Sets a GPIO pin to the specified value
static void setGPIO(FILE* loc, int toSet) {
   fprintf(loc, "%d", toSet);
   fflush(loc);
}

//Takes a GPIO pin number and returns a handle to a file where the GPIO pin can be controlled
static FILE* initGPIO(int gpioNum) {
   FILE *dir, *sys, *gpio;

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
   fseek(gpio, 0, SEEK_SET);
   return gpio;
}

//creates a pwmAttr and sets up member files for future access
static pwmAttr initPWM(int chipNum, int subID) {
   pwmAttr attrib;

   char exportC[256];
   sprintf(exportC, "/sys/class/pwm/pwmchip%d/export", chipNum);
   FILE *PWM1 = fopen(exportC, "w");
   fprintf(PWM1, "%d", subID);
   fflush(PWM1);
   fclose(PWM1);

   char periodC[256];
   sprintf (periodC, "/sys/class/pwm/pwmchip%d/pwm%d/period", chipNum, subID);
   attrib.period = fopen(periodC, "w");

   char dutyC[256];
   sprintf (dutyC, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", chipNum, subID);
   attrib.duty_cycle = fopen(dutyC, "w");

   char enableC[256];
   sprintf (enableC, "/sys/class/pwm/pwmchip%d/pwm%d/enable", chipNum, subID);
   printf ("/sys/class/pwm/pwmchip%d/pwm%d/enable\n", chipNum, subID);
   attrib.enable = fopen(enableC, "w");

   return attrib;
}