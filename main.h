
#ifndef MOTOR_ADC_MAIN_H_
#define MOTOR_ADC_MAIN_H_
 	#include <stdio.h>
	#include <stdlib.h> 
	#include <unistd.h>
	#include <time.h>
	#include <stropts.h>
	#include <poll.h>
	#include <fcntl.h>
	#include <sys/stat.h>
	#include <sys/types.h>
	#include <stdio.h>
	#include <assert.h>
	#include <poll.h>
	#include <signal.h>
	#include <stdio.h>
	#include <sys/signalfd.h>
	#include <string.h>

	//listening for adc or button interrupts, poll isnt' actually polling
	#define NUMPOLL 2


	//Directions
	#define OFF 0
	#define FORWARD 1
	#define BACKWARD 2
	#define BRAKE 3

	//MOTORS
	#define M_RIGHT 0
	#define M_LEFT 1

	//0 : normal
	//1 : front obstacle
	//2 : left obstacle
	//3 : right obstacle
	//state machine for car behavior, coupled with appropriate actions for entering each state
	typedef struct CarState {
		int state;
		int oldstate;
		void (* actions[10])(void);
	} CarState;

	//Function handles that are pointed to by the carstate, predefine movement patterns
	void cmdCarFwdFull(void);
	void cmdCarStop(void);
	//turn in place
	void cmdCarLeftFull(void);
	void cmdCarRightFull(void);
	//smoorth turn
	void cmdCarLeftFwd(void);
	void cmdCarRightFwd(void);

	//send motor command down pipe to motor process
	void setMotor(int whichMotor, int direction, int percent);
#endif