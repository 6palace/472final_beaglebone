//mainModule.c
//EE 472 Group F

#include "mainMod_sr.h"
#include "shiftReg.h"

static int __init driver_entry(void) {
   int ret;
   //Make a device class
   modclass = class_create(THIS_MODULE, CLASS_NAME);

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
   
   //Initalize Shift Register
   initSR();

   return 0;
}

static void __exit driver_exit(void) {
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

ssize_t device_read(struct file* filp, char* bufStoreData, 
   size_t bufLength, loff_t* curOffset) {
   return 0;
}

ssize_t device_write(struct file* filp, const char* bufSource,
   size_t bufLength, loff_t* curOffset) {
   printk(KERN_INFO DEVICE_NAME": writing %x to register\n", bufSource[0]);
   sendByte(bufSource[0]);
   return bufLength;
}

EXPORT_SYMBOL(sendByte);
MODULE_LICENSE("GPL"); // module license: required to use some functionalities.
module_init(driver_entry); // declares which function runs on init.
module_exit(driver_exit);  // declares which function runs on exit.
