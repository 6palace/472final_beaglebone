//adc.c
//EE472 Group F Lab 3

#include "adc.h"      
int main() {

   //init adc beaglebone
   //echo BB-ADC > /sys/devices/platform/bone_capemgr/slots
   FILE* bbCape = fopen("/sys/devices/platform/bone_capemgr/slots", "w");
   fprintf(bbCape, "BB-ADC");
   fflush(bbCape);
   fclose(bbCape);

   printf("setup complete\n");

   

   struct sigevent    sigx;
   struct sigaction   sigAct;
   struct itimerspec  newTime;
   timer_t timer;

   sigemptyset(&sigAct.sa_mask);
   sigAct.sa_flags = 0;
   sigAct.sa_handler = (void (*))timeHandler;
   sigaction(SIGUSR1, &sigAct, 0);


   // printf("signal setup\n");

   sigx.sigev_notify          = SIGEV_SIGNAL;
   sigx.sigev_signo           = SIGUSR1;
   sigx.sigev_value.sival_int = TIMER_TAG;

   newTime.it_value.tv_sec     = 1; 
   newTime.it_value.tv_nsec    = 2;
   newTime.it_interval.tv_sec  = 0;
   newTime.it_interval.tv_nsec = 100000000;

   timer_create(CLOCK_REALTIME, &sigx, &timer);
   timer_settime(timer, 0,  &newTime, NULL);

   while(1)
      pause();

   return 0;
}


static int readADC(int whichADC) {
   // printf("readADC\n");
	FILE* gpio;
	int adcVal;
	char valueLoc[256];
	sprintf (valueLoc, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", whichADC);
	gpio = fopen(valueLoc, "r");
	fscanf(gpio, "%d", &adcVal);
	fclose(gpio);
	return adcVal;
}

static void timeHandler(int sigNo, siginfo_t * evp, void * ucontext) {
   // printf("timeHandler\n");
   adcOut = fopen("/tmp/adcData", "w");
   fprintf(adcOut, "%d,%d,%d,%d\n", readADC(0), readADC(1), readADC(2), readADC(3));
   fclose(adcOut);
}
