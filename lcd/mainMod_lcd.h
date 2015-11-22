// mainModule.h
//EE 472 Group F

#ifndef NEW_CHAR_H_
#define NEW_CHAR_H_
 
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/device.h> 
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "lcd_char"
#define CLASS_NAME "gpio_lcd"

/* data structures */
// contains data about the device.
// data : buffer for character data stored.
// sem  : semaphore.
struct device_info {
	struct semaphore sem;
	int mode;
	int lineFlag;
	int cur_cust_char;
} virtual_device;

/* global variables */
// stores info about this char device.
static struct cdev* mcdev;
// holds major and minor number granted by the kernel
static dev_t dev_num;
static dev_t dev_ctrl_num;
static dev_t lcdSpCharDev[8];
// class of module for registration
static struct class *modclass; 

//===File operations===

// runs on startup
// intializes module space and declares major number.
// assigns device structure data.
static int __init driver_entry(void);

// called up exit.
// unregisters the device and all associated gpios with it.
static void __exit driver_exit(void);

// Called on device file open
//	inode reference to file on disk, struct file represents an abstract
// checks to see if file is already open (semaphore is in use)
// prints error message if device is busy.
int  device_open(struct inode*, struct file*);

// Called upon close
// closes device and returns access to semaphore.
int device_close(struct inode*, struct file *);

// Called when user wants to get info from device file
ssize_t device_read(struct file*, char*, size_t, loff_t*);

// Called when user wants to send info to device
ssize_t device_write(struct file*, const char*, size_t, loff_t*);
//stub method, no function
ssize_t device_read_control(struct file*, char*, size_t, loff_t*);

//parses utility commands into lcd, such as shifting screen, navigating cursor, 
//clearing, returning to home pos, etc.
/*
	0 OR 1 . clear
	2. turn cursor off
	3. blink and underline cursor mode
	4. blinking cursor mode
	5. underline cursor mode
	6. move display right by one
	7. move display left by one
	8. shifts cursor right every input character
	9. shifts text right instead of cursor
	11: total reset
	32-64. shift cursor to specified position
		32-47 being top row positions
		48-63 being second row positions

*/ 
ssize_t device_write_control(struct file*, const char*, size_t, loff_t*);

//Prints the specified character to the next position on the LCD
ssize_t device_read_custchar(struct file*, char*, size_t, loff_t*);

//takes in the string, pulls first 8 characters and uploads them into customchar
ssize_t device_write_custchar(struct file*, const char*, size_t, loff_t*);

/* operations usable by this file. */
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_close
};

/* operations usable by the control file file. */
static struct file_operations fops_control = {
	.owner = THIS_MODULE,
	.read = device_read_control,
	.write = device_write_control,
	.open = device_open,
	.release = device_close
};

/*operations usable by custom-char file*/
static struct file_operations fops_custchar = {
	.owner = THIS_MODULE,
	.read = device_read_custchar,
	.write = device_write_custchar,
	.open = device_open,
	.release = device_close
};

#endif
