//EE 472 Group F
#define BUTTON1 20
#define BUTTON2 70 //hardware redirect from 7
#define BUTTON3 106

//old file operation to set up gpio character driver
void initGPIO(int gpioNum);
//read value from switch GPIO
int readGPIO(int gpioNum);
//print string to lcd, including custom characters
void lcdPrint(char* str);
//send command to lcd, detailed in lcdsr.h
void lcdControl(char* str);
//play 1 round of space blasters
int playGame(void);

//random string that happened to look good
char enemy[8] = "fhidusor";

//2-part ship design
char ship0[8] = {
  0b100000,
  0b10000,
  0b11110,
  0b11011,
  0b11110,
  0b10000,
  0b00000,
  0b00000
};

char ship1[8] = {
  0b01100,
  0b01111,
  0b01111,
  0b01111,
  0b01111,
  0b01111,
  0b01100,
  0b00000
};
