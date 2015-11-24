/*
 * GPIO INTERRUPT
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>                 // Required for the GPIO functions
#include <linux/interrupt.h>            // Required for the IRQ code
#include <asm/siginfo.h>   //siginfo
#include <linux/rcupdate.h>   //rcu_read_lock
#include <linux/sched.h>   //find_task_by_pid_type
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <linux/device.h> 
#include <asm/uaccess.h>

 #define SIG_TEST 44
#define DEVICE_NAME "buttons_char"
#define CLASS_NAME "gpio_button_sig"

/* data structures */
// contains data about the device.
struct device_info {
   struct semaphore sem;
} virtual_device;

static ssize_t write_pid(struct file *file, const char __user *buf,
                                size_t count, loff_t *ppos);
int device_open(struct inode *inode, struct file* filp);
ssize_t device_read(struct file* filp, char* bufStoreData, 
   size_t bufLength, loff_t* curOffset);

int device_close(struct inode* inode, struct  file *filp);
static const struct file_operations fops = {
   .owner = THIS_MODULE,
   .write = write_pid,
   .open = device_open,
   .read = device_read,
   .release = device_close
};

/* global variables */
// stores info about this char device.
static struct cdev* mcdev;
// holds major and minor number granted by the kernel
static dev_t dev_num;
// class of module for registration
static struct class *modclass; 

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Button/LED test driver for the BBB");
MODULE_VERSION("0.1");

static unsigned int gpioButtons[] = {20, 75, 106};   ///< hard coding the button gpios
//75 points to empty gpio, since button1 shares output with pwm and can no longer be accessed
static unsigned int irqNumber[3];          ///< Used to share the IRQ number within this file
static unsigned int numberPresses = 0;  ///< For information, store the number of button presses
static int attachedPID;

/// Function prototype for the custom IRQ handler function -- see below for the implementation
static irq_handler_t  ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point. In this example this
 *  function sets up the GPIOs and the IRQ
 *  @return returns 0 if successful
 */
static int __init ebbgpio_init(void){
   int result = 0;
   //TODO create device file that user sends their pid to
   modclass = class_create(THIS_MODULE, CLASS_NAME);
   result |= alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
   if (result < 0) {
      printk(KERN_ALERT DEVICE_NAME": Failed to allocate a major number\n");
      return result;
   }
   printk(KERN_ALERT DEVICE_NAME": device major number inited\n");
   //Initialize cdev
   mcdev = cdev_alloc();
   mcdev->ops = &fops;
   mcdev->owner = THIS_MODULE;


   device_create(modclass, NULL, dev_num, NULL, DEVICE_NAME);

   sema_init(&virtual_device.sem, 1);

   printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST LKM\n");

   //initializing gpios
   for(int i = 0; i < 3; i++){
      gpio_request(gpioButtons[i], "sysfs");       // Set up the gpioButton
      gpio_direction_input(gpioButtons[i]);        // Set the button GPIO to be an input
      gpio_set_debounce(gpioButtons[i], 200);      // Debounce the button with a delay of 200ms
      gpio_export(gpioButtons[i], false);          // Causes gpio115 to appear in /sys/class/gpio
                             // the bool argument prevents the direction from being changed
      // Perform a quick test to see that the button is working as expected on LKM load
      printk(KERN_INFO "GPIO_TEST: The button %d state is currently: %d\n", i, gpio_get_value(gpioButtons[i]));

      // GPIO numbers and IRQ numbers are not the same! This function performs the mapping for us
      irqNumber[i] = gpio_to_irq(gpioButtons[i]);
      printk(KERN_INFO "GPIO_TEST: The button %d is mapped to IRQ: %u\n", i, irqNumber[i]);
      // This next call requests an interrupt line
      result |= request_irq(irqNumber[i],             // The interrupt number requested
                           (irq_handler_t) ebbgpio_irq_handler, // The pointer to the handler function below
                           IRQF_TRIGGER_RISING,   // Interrupt on rising edge (button press, not release)
                           "ebb_gpio_handler",    // Used in /proc/interrupts to identify the owner
                           NULL);                 // The *dev_id for shared interrupt lines, NULL is okay
   }



   printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", result);
   return result;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required. Used to release the
 *  GPIOs and display cleanup messages.
 */
static void __exit ebbgpio_exit(void){
   //printk(KERN_INFO "GPIO_TEST: The button state is currently: %d\n", gpio_get_value(gpioButton));
   printk(KERN_INFO "GPIO_TEST: Buttons were pressed %d times\n", numberPresses);
   for(int i = 0; i < 3; i++){
      free_irq(irqNumber[i], NULL);               // Free the IRQ number, no *dev_id required in this case
      gpio_unexport(gpioButtons[i]);               // Unexport the Button GPIO
      gpio_free(gpioButtons[i]);                   // Free the Button GPIO
   }
   
   printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n");
}

//requires a getpid() written from test program to kernel module
static ssize_t write_pid(struct file *file, const char __user *buf,
                                size_t count, loff_t *ppos){
   char mybuf[10];
   ssize_t res;

   /* read the value from user space */
   if(count > 10)
      return -EINVAL;
   res = copy_from_user(mybuf, buf, count);
   sscanf(mybuf, "%d", &attachedPID);
   printk("pid = %d\n", attachedPID);

   return count;
}

int device_open(struct inode *inode, struct file* filp) {
   if (down_interruptible(&virtual_device.sem) != 0) {
      printk(KERN_ALERT DEVICE_NAME": could not lock device during open\n");
      return -1;
   }
   filp->f_op = &fops; 
   return 0;
}


ssize_t device_read(struct file* filp, char* bufStoreData, 
   size_t bufLength, loff_t* curOffset) {
   return 0;
}

int device_close(struct inode* inode, struct  file *filp) {
   up(&virtual_device.sem);
   printk(KERN_INFO DEVICE_NAME": closing device\n");
   return 0;   
}
/** @brief The GPIO IRQ Handler function
 *  This function is a custom interrupt handler that is attached to the GPIO above. The same interrupt
 *  handler cannot be invoked concurrently as the interrupt line is masked out until the function is complete.
 *  This function is static as it should not be invoked directly from outside of this file.
 *  @param irq    the IRQ number that is associated with the GPIO -- useful for logging.
 *  @param dev_id the *dev_id that is provided -- can be used to identify which device caused the interrupt
 *  Not used in this example as NULL is passed.
 *  @param regs   h/w specific register values -- only really ever used for debugging.
 *  return returns IRQ_HANDLED if successful -- should return IRQ_NONE otherwise.
 */
static irq_handler_t ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
   int selectedButton = -1;
   struct siginfo info;
   struct task_struct *t;

   for(int i = 0; i < 3; i++){
      if(irq == irqNumber[i]){
         selectedButton = i;
         break;
      }
   }
   if(selectedButton > -1){
      printk(KERN_INFO "GPIO_TEST: Interrupt! (gpio %d is %d)\n", selectedButton, gpio_get_value(gpioButtons[selectedButton]));
   }
   numberPresses++;                         // Global counter, will be outputted when the module is unloaded


   memset(&info, 0, sizeof(struct siginfo));
   info.si_signo = SIG_TEST;
   info.si_code = SI_QUEUE; //possibly change to SI_QUEUE
   info.si_int = selectedButton;        //real time signals may have 32 bits of data.

   rcu_read_lock();
   t = pid_task(find_pid_ns(attachedPID, &init_pid_ns), PIDTYPE_PID);
   if(t == NULL){
      printk("no such pid\n");
      rcu_read_unlock();
      return (irq_handler_t) IRQ_HANDLED;
   }  
   rcu_read_unlock();


   send_sig_info(SIG_TEST, &info, t);
   return (irq_handler_t) IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}

/// This next calls are  mandatory -- they identify the initialization function
/// and the cleanup function (as above).
module_init(ebbgpio_init);
module_exit(ebbgpio_exit);