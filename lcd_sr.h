//lcd.h
//EE 472 Group F

#ifndef LCD_CHAR_H
#define LCD_CHAR_H
#include <linux/gpio.h>
#include <linux/delay.h>

//width of lcd screen in characters
#define LCDcharWidth 16

#define LCDreset 0x30
#define LCDclear 0x01
#define LCDentry_newchar_right 0x06
#define LCDentry_newchar_noshift 0x05
#define LCDentry_shift 0x07
#define LCDinit_format 0x38
#define LCDhome 0x02
#define LCDgoto_line1 	0b10000000
#define LCDgoto_line2 	0b11000000
#define LCDdata_shift_right 	0b00011100
#define LCDdata_shift_left 	0b00011000
#define LCDcursor_off 0x0c
#define LCDcursor_both 0x0f
#define LCDcursor_blink 0x0d
#define LCDcursor_underline 0x0e
#define LCDcursor_right 0x10

#define lcdE 69
#define lcdRW 67
#define lcdRS 66


//performs init/reset sequence on LCD
void lcdInit(void);
//init gpio port for software use
void initGPIO(int gpioNum);
//sets lcd data pin values to char
void lcdSendData(char data);
//perform command sequence
void lcdCommand(char data);
//perform write-to-lcd sequence
void lcdWrite(char data);
//subroutine for sending data
void lcdSend(void);
//modify cursor position to coordinates
void lcdGoTo(int x, int y);

//Uploads a custom character to locations 0-7. Assumes pattern is a 5x8bit array
//NOTE: must return home after character upload
void lcdCustomCharUpload(int location, const char* pattern);

extern void sendByte(char);

#endif