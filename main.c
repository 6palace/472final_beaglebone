//EE472 Group F Lab 3
#include "main.h"
#include <semaphore.h>
#include <regex.h>
//pthread_t tid[2];
//int counter;
//pthread_sem_name_t lock;
sem_t sem_name;

int main() {
   printf("Control Program\n");

   system("rm /tmp/adcData");
   system("rm /tmp/motorData");
   system("rm /tmp/fromWebsocket");
   system("rm /tmp/i2cData");
   mknod("/tmp/adcData", S_IFIFO, 0);
   mknod("/tmp/motorData", S_IFIFO, 0);
   mknod("/tmp/fromWebsocket", S_IFIFO, 0);
      mknod("/tmp/i2cData", S_IFIFO, 0);

   //pthread_sem_name_init(&lock, NULL);
   sem_init(&sem_name, 0, 1);

   pid_t pid = fork(); 
   if (pid < 0) 
      printf("Couldn't launch process\n");
   else if (pid == 0) {
   //MAIN PROGRAM

   struct pollfd pfd[NUMPOLL];
   int ret;
   ssize_t bytes;
   int fd, fd2, fd3, fd4;
   char* tok;
   int motorflag;
   fd = open("/tmp/adcData", O_RDONLY);
   fd2 = open("/dev/ib", O_RDONLY);
   fd3 = open("/tmp/fromWebsocket", O_RDWR);
   fd4 = open("/tmp/i2cData", O_RDWR);
   pfd[0].fd = fd;   
   pfd[0].events = POLLIN;
   pfd[1].fd = fd2;
   pfd[1].events = POLLIN;
   pfd[2].fd = fd3;
   pfd[2].events = POLLIN;
   pfd[3].fd = fd4;
   pfd[3].events = POLLIN;



   

   int adcVals[4]; //[0] back [1] left [2] front [3] right

    CarState cs;
    initCarState(&cs);

   regex_t ttregex;
   regcomp(&ttregex, "^TANKTURN:([-]?[0-9][0-9]?[0-9]?)$", REG_EXTENDED);

    printf("Entering main loop\n");

   long turnOffset = 0;
   int areWeGoingForward = 0;
   while(1) {

      char databuf[1024] = "";
      ret = poll(pfd, NUMPOLL, -1);
      if(ret > 0) { //Something happened
         if (pfd[0].revents & POLLIN) {
            if (cs.autoMode) {
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
         }

         if (pfd[1].revents & POLLIN) {
            //turn off cart motors when button is pressed
            read(fd2, databuf, 2);
            printf("Button: %s\n", databuf);
            cs.state = 4;
            cs.oldstate = 4;
            cs.actions[cs.state]();
         }

         if (pfd[2].revents & POLLIN) {
            read(fd3, databuf, 1000); //TODO FIX CHARS
            tok = strtok(databuf, ",\n");
            printf("Bwebsockin %s\n", tok);
            if(!strcmp(tok, "TANKFW")) {
               cmdCarFwdFull(turnOffset);
               areWeGoingForward = 1;
            }
            else if(!strcmp(tok, "TANKREV"))
               cmdCarBackFull();
            else if(!strcmp(tok, "TANKSTOP")) {
               areWeGoingForward = 0;
               cmdCarStop();
            }

             //TANKREV, TANKSTOP

            regmatch_t pmatch[2];
            int ret = regexec(&ttregex, tok, 2, pmatch, 0);
            if (!ret && areWeGoingForward) { //TankTurn
               char* pend;
               turnOffset = strtol(tok+pmatch[1].rm_so, &pend, 10);
               cmdCarFwdFull(turnOffset);
            }
            
         }

          if (pfd[3].revents & POLLIN) {

             FILE* toWebsocket = fopen("/tmp/toWebsocket", "w");
             read(fd4, databuf, 20);
             printf("i2C: %s\n", databuf);
             fprintf(toWebsocket, "%s\n", databuf);
             fflush(toWebsocket);

             fclose(toWebsocket);
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

//chang ethis
static void setMotor(int direction, int percent, int direction2, int percent2) {
   FILE* motorOut = fopen("/tmp/motorData", "w");
   fprintf(motorOut, "%d,%d,%d,%d\n", direction, percent, direction2, percent2);
   // printf("To Motor: %d,%d,%d\n", whichMotor, direction, percent);
   usleep(2000);
   fflush(motorOut);
   fclose(motorOut);
   usleep(2000);
}



static void cmdCarFwdFull(int turn){
      sem_wait (&sem_name);
      printf("Car go!\n");

      int left, right;

      if (turn < 0) {
         left = turn;
         right = 0; 
      } else {
         right = turn;
         left = 0;
      }

      setMotor(FORWARD, 100 + left, FORWARD, 100 - right);
      sem_post(&sem_name);
}

static void cmdCarStop(void){
      sem_wait (&sem_name);
      printf("Car stop!\n");
      //setMotor(M_RIGHT, OFF, 80);
      //setMotor(M_LEFT, OFF, 80);
      setMotor(OFF, 80, OFF, 80);
      sem_post(&sem_name);
}

static void cmdCarLeftFull(void){
      sem_wait (&sem_name);
      printf("Car rotate left!\n");
      //setMotor(M_RIGHT, FORWARD, 80);
      //setMotor(M_LEFT, BACKWARD, 80);
      setMotor(BACKWARD, 80, FORWARD, 80);
      sem_post(&sem_name);
}

static void cmdCarLeftFwd(void){
      sem_wait (&sem_name);
      printf("Car sweep left!\n");
      //setMotor(M_RIGHT, FORWARD, 80);
      //setMotor(M_LEFT, FORWARD, 45);
      setMotor(FORWARD, 45, FORWARD, 80);
      sem_post(&sem_name);
}

static void cmdCarRightFull(void){
      sem_wait (&sem_name);
      printf("Car rotate right!\n");
      //setMotor(M_RIGHT, BACKWARD, 80);
      //setMotor(M_LEFT, FORWARD, 80);
      setMotor(FORWARD, 80, BACKWARD, 80);
      sem_post(&sem_name);
}

static void cmdCarBackFull(void){
      sem_wait (&sem_name);
      printf("Car back!\n");
      //setMotor(M_RIGHT, BACKWARD, 80);
      //setMotor(M_LEFT, BACKWARD, 80);
      setMotor(BACKWARD, 80, BACKWARD, 80);
      sem_post(&sem_name);
}

static void cmdCarRightFwd(void){
      sem_wait (&sem_name);
      printf("Car sweep right!\n");
      //setMotor(M_RIGHT, FORWARD, 45);
      //setMotor(M_LEFT, FORWARD, 80);
      setMotor(FORWARD, 45, FORWARD, 80);
      sem_post(&sem_name);
}

//initialize state to full forward, create action pointer values
static void initCarState(CarState* state){
   state->state = 0;
   state->autoMode = 0;
   state->actions[0] = cmdCarFwdFull;
   state->actions[1] = cmdCarRightFull;
   state->actions[3] = cmdCarRightFwd;
   state->actions[2] = cmdCarLeftFwd;
   state->actions[4] = cmdCarStop;
    //state->actions[state->state]();
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
//duty percent 30 minimum