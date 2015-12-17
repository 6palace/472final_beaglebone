
//EE472 Group F Lab 3
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
   #include <pthread.h>

   //listening for adc or button interrupts, poll isnt' actually polling
   #define NUMPOLL 3

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
      int autoMode;
   } CarState;

   //Function handles that are pointed to by the carstate, predefine movement patterns
   static void cmdCarFwdFull(void);
   static void cmdCarStop(void);
   //turn in place
   static void cmdCarLeftFull(void);
   static void cmdCarRightFull(void);
   //smooth turn
   static void cmdCarLeftFwd(void);
   static void cmdCarRightFwd(void);
   static void cmdCarBackFull(void);

   static void initCarState(CarState* state);
   static void updateCarState(CarState* state, int adcVals[4]);
   static void refreshCarState(CarState* state);

   //send motor command down pipe to motor process
   static void setMotor(int direction, int percent, int direction2, int percent2);
#endif