//lcd.c
//EE 472 Group F

#include "lcd_sr.h"

int lcdDevControl[3] = {lcdE, lcdRW, lcdRS};

void lcdInit(void) {

   for (int i = 0; i < 3; i++) {
      initGPIO(lcdDevControl[i]);
   }

   msleep(100);
   lcdCommand(LCDreset);
   msleep(30);
   lcdCommand(LCDreset);
   msleep(10);
   lcdCommand(LCDreset);
   msleep(10);
   lcdCommand(LCDinit_format);
   lcdCommand(LCDcursor_right);
   lcdCommand(LCDcursor_blink);
   lcdCommand(LCDentry_newchar_right);
   lcdCommand(LCDclear);
}

void lcdWrite(char data) {
   gpio_set_value(lcdDevControl[2], 1);
   gpio_set_value(lcdDevControl[1], 0);   
   lcdSendData(data);
   lcdSend();
}

void lcdCommand(char data) {
   gpio_set_value(lcdDevControl[2], 0);
   gpio_set_value(lcdDevControl[1], 0);   
   lcdSendData(data);
   lcdSend();
}

void lcdSendData(char dataIn) {
   sendByte(dataIn);
}

void lcdCustomCharUpload(int location, const char* pattern) {
   int i;
   for(i = 0; i < 8; i++) {
      gpio_set_value(lcdDevControl[2], 0); //RS
      gpio_set_value(lcdDevControl[1], 0); //RW
      lcdSendData(0b01000000 + location*8 + i); //Set CGRAM Address
      lcdSend();
      gpio_set_value(lcdDevControl[2], 1); //RS
      lcdSendData(0b00000000 + pattern[i]); //Write data to RAM
      lcdSend();
   }
}

void lcdSend(void) {
   gpio_set_value(lcdDevControl[0], 1);
   //msleep(1);
   mdelay(2);
   gpio_set_value(lcdDevControl[0], 0);
}

void lcdGoTo(int x, int y) {
   if (x == 0) {
      lcdCommand(LCDgoto_line1+y);
   } else {
      lcdCommand(LCDgoto_line2+y);
   }
}

void initGPIO(int gpioNum) {
   char labstr[] = "gpio";
   gpio_request(gpioNum, labstr);
   gpio_direction_output(gpioNum, 1);
   gpio_set_value(gpioNum, 0);
}
