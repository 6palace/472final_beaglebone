
#include "main.h"



   //Function handles that are pointed to by the carstate, predefine movement patterns
   static void cmdCarFwdFull(void);
   static void cmdCarStop(void);
   //turn in place
   static void cmdCarLeftFull(void);
   static void cmdCarRightFull(void);
   //smoorth turn
   static void cmdCarLeftFwd(void);
   static void cmdCarRightFwd(void);

//initialize state to full forward, create action pointer values
static void initCarState(CarState* state){
   state->state = 0;
   state->actions[0] = cmdCarFwdFull;
   state->actions[1] = cmdCarRightFull;
   state->actions[3] = cmdCarRightFwd;
   state->actions[2] = cmdCarLeftFwd;
   state->actions[4] = cmdCarStop;
    state->actions[state->state]();
}

//where behavior changing is written
static void updateCarState(CarState* state, int adcVals[4]){
   if(adcVals[2] > 1600){
      state->state = 1;
   } else{
      if(adcVals[1] > 1600){
         state->state = 2;
      } else if(adcVals[3] > 1600){
         state->state = 3;
      } else{
         state->state = 0;
      }
   }
}

static void refreshCarState(CarState* state){
   state->oldstate = state->state;
}

int main() {
   printf("Control Program\n");

   system("rm /tmp/adcData");
   system("rm /tmp/motorData");
   mknod("/tmp/adcData", S_IFIFO, 0);
   mknod("/tmp/motorData", S_IFIFO, 0);

   pid_t pid = fork(); 
   if (pid < 0) 
      printf("Couldn't launch process\n");
   else if (pid == 0) {
   //MAIN PROGRAM

   struct pollfd pfd[NUMPOLL];
   int ret;
   ssize_t bytes;
   int fd, fd2;
   char* tok;
   int motorflag;

   fd = open("/tmp/adcData", O_RDONLY);
   fd2 = open("/dev/ib", O_RDONLY);

   pfd[0].fd = fd;   
   pfd[0].events = POLLIN;
   pfd[1].fd = fd2;
   pfd[1].events = POLLIN;

   char databuf[1024] = "";

   
   int adcVals[4]; //[0] back [1] left [2] front [3] right

    CarState cs;
    initCarState(&cs);

   while(1) {
      ret = poll(pfd, NUMPOLL, -1);
      if(ret > 0) { //Something happened
         if (pfd[0].revents & POLLIN) {
            printf("ADC stored:\n");
            read(fd, databuf, 1024);
            tok = strtok(databuf, ",\n");
            int i = 0;
            while(i < 4 && tok != NULL) 
            {
               adcVals[i] = atoi(tok);
               printf(" %d\n", adcVals[i]);
               tok = strtok(NULL, ",\n");
               i++;
            }

            if(cs.oldstate != 4){
               //detect change in car state, send new commands down pipe
               updateCarState(&cs, adcVals);
               printf("curState: %d\n", cs.state);
               if(cs.state != cs.oldstate){
                  printf("new state: %d\n", cs.state);
                  cs.actions[cs.state]();
               }
               refreshCarState(&cs);
            }   
         }

         if (pfd[1].revents & POLLIN) {
            //turn off cart motors when button is pressed
            read(fd2, databuf, 2);
            printf("Button: %s\n", databuf);
            cs.state = 4;
            cs.oldstate = 4;
            cs.actions[cs.state]();
         }
      }
   }

   //END MAIN PROGRAM
   } else {
      //launch child processes
      pid_t pid2 = fork(); 
      if (pid2 < 0) 
         printf("Couldn't launch process\n");
      else if (pid2 == 0) {
         system ("./adc");
         printf("ADC\n");
      } else {
         system ("./motor");
         printf("MOTOR\n");
      }
   }
}


static void setMotor(int whichMotor, int direction, int percent) {
   FILE* motorOut = fopen("/tmp/motorData", "w");
   fprintf(motorOut, "%d,%d,%d\n", whichMotor, direction, percent);
   // printf("To Motor: %d,%d,%d\n", whichMotor, direction, percent);
   usleep(1000);
   fflush(motorOut);
   fclose(motorOut);
}


static void cmdCarFwdFull(void){
      printf("Car go!\n");
      setMotor(M_RIGHT, FORWARD, 80);
      setMotor(M_LEFT, FORWARD, 80);
}

static void cmdCarStop(void){
      printf("Car stop!\n");
      setMotor(M_RIGHT, OFF, 80);
      setMotor(M_LEFT, OFF, 80);
}

static void cmdCarLeftFull(void){
      printf("Car rotate left!\n");
      setMotor(M_RIGHT, FORWARD, 80);
      setMotor(M_LEFT, BACKWARD, 80);
}

static void cmdCarLeftFwd(void){
      printf("Car sweep left!\n");
      setMotor(M_RIGHT, FORWARD, 80);
      setMotor(M_LEFT, FORWARD, 45);
}

static void cmdCarRightFull(void){
      printf("Car rotate right!\n");
      setMotor(M_RIGHT, BACKWARD, 80);
      setMotor(M_LEFT, FORWARD, 80);
}

static void cmdCarRightFwd(void){
      printf("Car sweep right!\n");
      setMotor(M_RIGHT, FORWARD, 45);
      setMotor(M_LEFT, FORWARD, 80);
}
//duty percent 30 minimum