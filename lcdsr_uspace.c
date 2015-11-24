#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>  
#include "lcdsr_uspace.h"

int main() {
	char buffer1[200] = "0";
   int contin = 1;

	initGPIO(BUTTON1);
	initGPIO(BUTTON2);
	initGPIO(BUTTON3);

   lcdControl("11"); //clear any old LCD settings
   lcdControl("2"); //turn cursor off

   FILE* customChar1 = fopen("/dev/lcd_char/custom/char1", "w");
   fseek(customChar1, 0, SEEK_SET);
   fprintf(customChar1, "%s", ship1);
   fflush(customChar1);
   fclose(customChar1);
   FILE* customChar2 = fopen("/dev/lcd_char/custom/char2", "w");
   fseek(customChar2, 0, SEEK_SET);
   fprintf(customChar2, "%s", ship0);
   fflush(customChar2);
   fclose(customChar2);
   FILE* customChar3 = fopen("/dev/lcd_char/custom/char3", "w");
   fseek(customChar3, 0, SEEK_SET);
   fprintf(customChar3, "%s", enemy);
   fflush(customChar3);
   fclose(customChar3);


   lcdPrint(" SPACE BLASTER!\n  Press a key");

   while(!(readGPIO(BUTTON1) || readGPIO(BUTTON2) || readGPIO(BUTTON3))) {
     usleep(1);
   }

   
   while(contin){
      int allow = 0;
      lcdControl("0");
      srand(time(NULL));
      int score = 0;
      contin = 0;
      score = playGame();
      sprintf(buffer1, "    Score: %d", score);

      int gameTick = 0;
      int displayFlag = 0;
      while(1){
         gameTick++;
         if(allow && readGPIO(BUTTON2)){
            contin = 1;
            break;
         }
         if(allow && (readGPIO(BUTTON1) || readGPIO(BUTTON3))){
            contin = 0;
            break;
         }
         if(gameTick % 4000 == 1){
            displayFlag = !displayFlag;
            lcdControl("0");
            if(displayFlag){
               lcdPrint("   Game Over!!!\n");
               lcdPrint(buffer1);
            } else{
               lcdPrint("Fire to restart,\n");
               lcdPrint(" Other to quit");
               allow = 1;
            }
         }
         usleep(50);
      }
   }

	return 0;
}


void lcdPrint(char* str) {
   FILE* txt = fopen("/dev/lcd_char/text", "w");
   fprintf(txt, "%s", str);
   fclose(txt);
}

void lcdControl(char* str) {
   FILE* ctrl = fopen("/dev/lcd_char/ctrl", "w");
   fprintf(ctrl, "%s", str);
   fclose(ctrl);  
}

//Takes a GPIO pin number and returns a handle to a file where the GPIO pin can be controlled
void initGPIO(int gpioNum) {
	FILE *dir, *sys;

	sys = fopen("/sys/class/gpio/export", "w");
	fseek(sys, 0, SEEK_SET);
	fprintf(sys, "%d", gpioNum);
	fflush(sys);
	fclose(sys);

	char directionLoc[256];
	sprintf (directionLoc, "/sys/class/gpio/gpio%d/direction", gpioNum);
	dir = fopen(directionLoc, "w");
	fseek(dir, 0, SEEK_SET);
	fprintf(dir, "%s", "in");
	fflush(dir);
	fclose(dir);
}

int readGPIO(int gpioNum) {
	FILE* gpio;
	char strBuff[2];
	char valueLoc[256];
	sprintf (valueLoc, "/sys/class/gpio/gpio%d/value", gpioNum);
	gpio = fopen(valueLoc, "r");
	fseek(gpio, 0, SEEK_SET);
	fscanf(gpio, "%s", strBuff);
	fclose(gpio);
	return strcmp(strBuff, "1");
}

int playGame(void){
   int shipPos = 0;
   int oldPos = 3;
   int projectiles0[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   int projectiles1[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};

   int enemies0[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   int enemies1[14] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0};
   int frameTick = 0;
   FILE* txt, ctrl;
   int buttonStatus[3];
   int score = 0;
   
   while(1) {
      frameTick++;
      buttonStatus[0] = readGPIO(BUTTON1);
      buttonStatus[1] = readGPIO(BUTTON2);
      buttonStatus[2] = readGPIO(BUTTON3);
      printf("%d %d %d\b\b\b\b\b\b", //print button status to console
         buttonStatus[0], //Button 1
         buttonStatus[1], //Button 2
         buttonStatus[2]  //Button 3
      );

      //LCD Display Code
      //button 3 is ship down
      if (buttonStatus[2]) {
         shipPos = 1;
      }

      //button 1 is ship up
      if (buttonStatus[0]) {
         shipPos = 0;
      }

      //button 2 is fire
      if (buttonStatus[1]) {
         if (shipPos == 0)
            projectiles0[0] = 1;
         else 
            projectiles1[0] = 1;
      }

      //Draw Ship
      if (oldPos != shipPos) {
         if (shipPos == 0) {
            lcdControl("32");
            lcdPrint("\1\2");
            lcdControl("48");
            lcdPrint("  ");
         } else {
            lcdControl("48");
            lcdPrint("\1\2");
            lcdControl("32");
            lcdPrint("  ");
         }
         oldPos = shipPos;
      }
      
      
      if ((frameTick % 3) == 0) {
         //draw projectiles and enemies
         lcdControl("34");
         txt = fopen("/dev/lcd_char/text", "w");
         for (int i = 0; i < 14; i++) {
            if (projectiles0[i])
               fprintf(txt, "%s", "-");
            else if(enemies0[i])
               fprintf(txt, "%s", "\3");
            else
               fprintf(txt, "%s", " ");
         }      
         fclose(txt);
         lcdControl("50");
         txt = fopen("/dev/lcd_char/text", "w");
         for (int i = 0; i < 14; i++) {
            if (projectiles1[i])
               fprintf(txt, "%s", "-");
            else if(enemies1[i])
               fprintf(txt, "%s", "\3");
            else
               fprintf(txt, "%s", " ");
         } 
         fclose(txt);



         //enemies move slower than bullets
         if((frameTick % 2) == 0){
            //check gameOver
            if(enemies0[0] || enemies1[0]){
               break;
            }
            int randInstance = rand() % 4;
            // //shift enemies, checks for hits
            for (int i = 0; i <= 13; i++) {
               enemies0[i] = enemies0[i+1];
               enemies1[i] = enemies1[i+1];

               if(projectiles0[i] && enemies0[i]){
                  enemies0[i] = 0;
                  projectiles0[i] = 0;
                  score++;
               }
               if(projectiles1[i] && enemies1[i]){
                  enemies1[i] = 0;
                  projectiles1[i] = 0;
                  score++;
               }
            }


            //generate new enemies
            enemies0[13] = 0;
            enemies1[13] = 0;
            if(randInstance == 0){
               enemies0[13] = 1;
            } else if(randInstance == 1){
               enemies1[13] = 1;
            }
         }


         //shift projectiles
         for (int i = 13; i >= 1; i--) {
            projectiles0[i] = projectiles0[i-1];
            projectiles1[i] = projectiles1[i-1];

            if(projectiles0[i] && enemies0[i]){
               enemies0[i] = 0;
               projectiles0[i] = 0;
               score++;
            }
            if(projectiles1[i] && enemies1[i]){
               enemies1[i] = 0;
               projectiles1[i] = 0;
               score++;
            }

         }
         projectiles0[0] = 0;
         projectiles1[0] = 0;



      }
      usleep(200);
   }

   return score;
}