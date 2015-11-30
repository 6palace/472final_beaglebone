// interrupt.h
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
#include <linux/interrupt.h>            // Required for the IRQ code
#include <asm/siginfo.h>   //siginfo
#include <linux/rcupdate.h>   //rcu_read_lock
#include <linux/sched.h>   //find_task_by_pid_type

#define DEVICE_NAME "ib"
#define CLASS_NAME "inbu"

/* data structures */
// contains data about the device.
// data : buffer for character data stored.
// sem  : semaphore.
struct device_info {
	struct semaphore sem;
	int mode;
	int interruptWaiting;
	int buttonSent;
	int button;
} virtual_device;

/* global variables */
// stores info about this char device.
static struct cdev* mcdev;
// holds major and minor number granted by the kernel
static dev_t dev_num;
// class of module for registration
static struct class *modclass; 


static irq_handler_t ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);
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

/* operations usable by this file. */
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_close
};

#endif
