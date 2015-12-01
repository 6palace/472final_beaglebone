//interrupt.c
//EE 472 Group F

#include "interrupt.h"

//static unsigned int gpioLED = 74;       ///< hard coding the LED gpio for this example to P9_23 (GPIO49)
static unsigned int gpioButton = 20;   ///< hard coding the button gpio for this example to P9_27 (GPIO115)
static unsigned int gpioButton2 = 106;   ///< hard coding the button gpio for this example to P9_27 (GPIO115)
static unsigned int irqNumber;          ///< Used to share the IRQ number within this file
static unsigned int irqNumber2;          ///< Used to share the IRQ number within this file

static wait_queue_head_t wq;


static int __init driver_entry(void) {
   int ret, result, result2;
   //Make a device class
   modclass = class_create(THIS_MODULE, CLASS_NAME);

   //Init wait queue
   init_waitqueue_head(&wq);

   //Get major and minor number for device
   ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
   if (ret < 0) {
      printk(KERN_ALERT DEVICE_NAME": Failed to allocate a major number\n");
      return ret;
   }

   //Initialize cdev
   mcdev = cdev_alloc();
   mcdev->ops = &fops;
   mcdev->owner = THIS_MODULE;

   //Add character device to filesystem
   ret = cdev_add(mcdev, dev_num, 1);
   if (ret < 0) {
      printk(KERN_ALERT DEVICE_NAME": unable to add cdev to kernel\n");
      return ret;
   }
   device_create(modclass, NULL, dev_num, NULL, DEVICE_NAME);
   printk(KERN_INFO DEVICE_NAME": Created\n");

   // Initialize SEMAPHORE
   sema_init(&virtual_device.sem, 1);
   result = 0;
   result2 = 0;
   printk(KERN_INFO "GPIO_TEST: Initializing the GPIO_TEST LKM\n");
   // Is the GPIO a valid GPIO number (e.g., the BBB has 4x32 but not all available)
   gpio_request(gpioButton, "sysfs");       // Set up the gpioButton
   gpio_direction_input(gpioButton);        // Set the button GPIO to be an input
   gpio_set_debounce(gpioButton, 200);      // Debounce the button with a delay of 200ms
   gpio_export(gpioButton, false);          // Causes gpio115 to appear in /sys/class/gpio  
   gpio_request(gpioButton2, "sysfs");       // Set up the gpioButton
   gpio_direction_input(gpioButton2);        // Set the button GPIO to be an input
   gpio_set_debounce(gpioButton2, 200);      // Debounce the button with a delay of 200ms
   gpio_export(gpioButton2, false);          // Causes gpio115 to appear in /sys/class/gpio
                             // the bool argument prevents the direction from being changed
   // Perform a quick test to see that the button is working as expected on LKM load
   printk(KERN_INFO "GPIO_TEST: The button state is currently: %d\n", gpio_get_value(gpioButton));

   // GPIO numbers and IRQ numbers are not the same! This function performs the mapping for us
   irqNumber = gpio_to_irq(gpioButton);
   irqNumber2 = gpio_to_irq(gpioButton2);

   printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);

   // This next call requests an interrupt line
   result = request_irq(irqNumber,             // The interrupt number requested
                        (irq_handler_t) ebbgpio_irq_handler, // The pointer to the handler function below
                        IRQF_TRIGGER_RISING,   // Interrupt on rising edge (button press, not release)
                        "ebb_gpio_handler",    // Used in /proc/interrupts to identify the owner
                        NULL);                 // The *dev_id for shared interrupt lines, NULL is okay   
   result2 = request_irq(irqNumber2,             // The interrupt number requested
                        (irq_handler_t) ebbgpio_irq_handler, // The pointer to the handler function below
                        IRQF_TRIGGER_RISING,   // Interrupt on rising edge (button press, not release)
                        "ebb_gpio_handler",    // Used in /proc/interrupts to identify the owner
                        NULL);                 // The *dev_id for shared interrupt lines, NULL is okay

   printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", result);

   return 0;
}

static void __exit driver_exit(void) {
   printk(KERN_INFO "GPIO_TEST: The button state is currently: %d\n", gpio_get_value(gpioButton));
   free_irq(irqNumber, NULL);               // Free the IRQ number, no *dev_id required in this case
   free_irq(irqNumber2, NULL);               // Free the IRQ number, no *dev_id required in this case
   gpio_unexport(gpioButton);               // Unexport the Button GPIO
   gpio_unexport(gpioButton2);               // Unexport the Button GPIO
   gpio_free(gpioButton);                   // Free the Button GPIO
   gpio_free(gpioButton2);                   // Free the Button GPIO
   printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n");

   cdev_del(mcdev);
   unregister_chrdev_region(dev_num, 1);
   device_destroy(modclass, dev_num);
   class_destroy(modclass);
   printk(KERN_ALERT DEVICE_NAME": successfully unloaded\n");
}

int device_open(struct inode *inode, struct file* filp) {
   if (down_interruptible(&virtual_device.sem) != 0) {
      printk(KERN_ALERT DEVICE_NAME": could not lock device during open\n");
      return -1;
   }
   filp->f_op = &fops; 
   return 0;
}

int device_close(struct inode* inode, struct  file *filp) {
   up(&virtual_device.sem);
   printk(KERN_INFO DEVICE_NAME": closing device\n");
   return 0;   
}

static irq_handler_t ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs) {
   printk(KERN_INFO "GPIO_TEST: Interrupt! (button state is %d)\n", gpio_get_value(gpioButton));
   wake_up_interruptible(&wq); //Wake up any blocking reads
   virtual_device.button = (irq == irqNumber);
   virtual_device.interruptWaiting = 1;
   return (irq_handler_t) IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}

ssize_t device_read(struct file* filp, char* bufStoreData, 
   size_t bufLength, loff_t* curOffset) {
   int ret;
   char* buttonStr ;

   printk("offset is at %d\n", *curOffset);
   if(*curOffset > 0)  {
      *curOffset = 0;
      return 0;
   }

   if (virtual_device.button)
      buttonStr = "1";
   else
      buttonStr = "0";

   ret = copy_to_user(bufStoreData, buttonStr, strlen(buttonStr)+1);
   *curOffset += strlen(buttonStr);
   virtual_device.interruptWaiting = 0;
   return strlen(buttonStr);   
}

int device_poll(struct file* filp , poll_table* wait) {
   unsigned int mask;
   printk("Poll was called\n");

   poll_wait(filp, &wq, wait);

   mask = 0;
   if (virtual_device.interruptWaiting)
      mask |= POLLIN | POLLRDNORM;

   return mask;
}


ssize_t device_write(struct file* filp, const char* bufSource,
   size_t bufLength, loff_t* curOffset) {
   printk(KERN_INFO DEVICE_NAME": Not Implemented\n");
   return bufLength;
}

MODULE_LICENSE("GPL"); // module license: required to use some functionalities.
module_init(driver_entry); // declares which function runs on init.
module_exit(driver_exit);  // declares which function runs on exit.
