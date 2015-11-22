//mainModule.c
//EE 472 Group F

#include "mainMod_lcd.h"
#include "lcd_sr.h"

static int __init driver_entry(void) {
   int ret, i;
   //Make a device class
   modclass = class_create(THIS_MODULE, CLASS_NAME);

   //Get major and minor number for device
   ret = alloc_chrdev_region(&dev_num, 0, 10, DEVICE_NAME);
   if (ret < 0) {
      printk(KERN_ALERT DEVICE_NAME": Failed to allocate a major number\n");
      return ret;
   }

   dev_ctrl_num = MKDEV(MAJOR(dev_num), MINOR(dev_num)+1);
   //Init special character devices at minor numbers 2 - 10
   for (i = 0; i < 8; i++) {
      lcdSpCharDev[i] = MKDEV(MAJOR(dev_num), MINOR(dev_num)+2+i);
   }

   //Initialize cdev
   mcdev = cdev_alloc();
   mcdev->ops = &fops;
   mcdev->owner = THIS_MODULE;

   //Add character device to filesystem
   ret = cdev_add(mcdev, dev_num, 10);
   if (ret < 0) {
      printk(KERN_ALERT DEVICE_NAME": unable to add cdev to kernel\n");
      return ret;
   }

   device_create(modclass, NULL, dev_num, NULL, DEVICE_NAME"/text");
   device_create(modclass, NULL, dev_ctrl_num, NULL, DEVICE_NAME"/ctrl");
   for (i = 0; i < 8; i++) {
      char devicePath[128]; 
      sprintf(devicePath, DEVICE_NAME"/custom/char%d", i);
      device_create(modclass, NULL, lcdSpCharDev[i], NULL, devicePath);
   }

   printk(KERN_INFO DEVICE_NAME": Created\n");

   // Initialize SEMAPHORE
   sema_init(&virtual_device.sem, 1);
   
   // Init LCD Hardware
   printk(KERN_INFO DEVICE_NAME": Starting LCD Initialization\n");
   lcdInit();
   return 0;
}

static void __exit driver_exit(void) {
   int i;
   cdev_del(mcdev);
   unregister_chrdev_region(dev_num, 10);
   device_destroy(modclass, dev_num);
   device_destroy(modclass, dev_ctrl_num);
   for (i = 0; i < 8; i++) {
      device_destroy(modclass, lcdSpCharDev[i]);
   }
   class_destroy(modclass);
   printk(KERN_ALERT DEVICE_NAME": successfully unloaded\n");
}

int device_open(struct inode *inode, struct file* filp) {
   int minor_num;
   if (down_interruptible(&virtual_device.sem) != 0) {
      printk(KERN_ALERT DEVICE_NAME": could not lock device during open\n");
      return -1;
   }
   minor_num = MINOR(inode->i_rdev);
   printk(KERN_INFO DEVICE_NAME": device opened. Minor Number:%d\n", 
      minor_num);
   if(minor_num == 0){
      filp->f_op = &fops; 
   } else if(minor_num == 1){
      filp->f_op = &fops_control;
   } else if(minor_num >= 2 && minor_num <= 10){
      virtual_device.cur_cust_char = minor_num - 2;
      filp->f_op = &fops_custchar;
   }
   return 0;
}

int device_open_control(struct inode *inode, struct file* filp) {
   return 0;
}

int device_close(struct inode* inode, struct  file *filp) {
   up(&virtual_device.sem);
   printk(KERN_INFO DEVICE_NAME": closing device\n");
   return 0;   
}

ssize_t device_read(struct file* filp, char* bufStoreData, 
   size_t bufLength, loff_t* curOffset) {
   int ret;
   char funStr[] = "NOTE: check LCD fluid.\n";
   if(*curOffset > 0) return 0; //Don't read past the end of the file.
   ret = copy_to_user(bufStoreData, funStr, strlen(funStr));
   *curOffset += strlen(funStr);
   return strlen(funStr);
}

ssize_t device_write(struct file* filp, const char* bufSource,
   size_t bufLength, loff_t* curOffset) {
   int i, cursorPos;

   printk(KERN_INFO DEVICE_NAME": writing to device...\n");
   printk(KERN_INFO DEVICE_NAME": Received %d characters\n", (int)bufLength);

   cursorPos = 0;

   switch(virtual_device.mode) {
      case 0: //Handles new lines, does not scroll, erases both lines when full.
         for(i = 0; i < bufLength; i++) {
            if (bufSource[i] == '\n'){
               if(virtual_device.lineFlag){
                  lcdCommand(LCDgoto_line1);
                  lcdCommand(LCDclear);
                  virtual_device.lineFlag = 0;
               } else {
                  lcdCommand(LCDgoto_line2);
                  virtual_device.lineFlag = 1;
               }
               cursorPos = 0;
            } else {
               if (cursorPos < LCDcharWidth*2) {
                  lcdWrite(bufSource[i]);
                  cursorPos++;
               }
            }
         }
         break;
   }
   
   return bufLength;
}

ssize_t device_write_control(struct file* filp, const char* bufSource,
   size_t bufLength, loff_t* curOffset) {
   int x, y, cmdIn, res;
   int i = 0;
   char tempBuf[50];
   if (bufLength > 40) {
      return 0;
   }
   while((bufSource[i] >= '0' && bufSource[i] <= '9') && i <= 40){
      tempBuf[i] = bufSource[i];
      i++;
   }
   tempBuf[i] = '\0';
   res = kstrtoint(tempBuf, 10, &cmdIn);
   printk(KERN_INFO DEVICE_NAME": command %d received\n", cmdIn);
   switch (cmdIn) {
      case 0:
         virtual_device.mode = cmdIn;
      case 1: //clear and reset software line flag
         virtual_device.lineFlag = 0;
         lcdCommand(LCDclear);
         break;
      case 2:
         lcdCommand(LCDcursor_off);
         break;
      case 3:
         lcdCommand(LCDcursor_both);
         break;
      case 4:
         lcdCommand(LCDcursor_blink);
         break;
      case 5:
         lcdCommand(LCDcursor_underline);
         break;
      case 6:
         lcdCommand(LCDdata_shift_left);
         break;
      case 7:
         lcdCommand(LCDdata_shift_right);
         break;
      case 8: //autoshift right
         lcdCommand(LCDentry_newchar_right);
         break;
      case 9:
         lcdCommand(LCDentry_newchar_noshift);
         break;
      case 11: //total reset
         virtual_device.lineFlag = 0;
         lcdInit();
         break;
      default:
         if (cmdIn > 31 && cmdIn < 64) {
            virtual_device.lineFlag = 0;
            x = (cmdIn - 32) / 16;
            y = (cmdIn - 32) % 16;
            lcdGoTo(x, y);
            virtual_device.lineFlag = x;
            printk(KERN_INFO DEVICE_NAME": moving to: %d %d", x, y);
         } else {
            printk(KERN_INFO DEVICE_NAME": invalid command: %d", cmdIn);
         }
   }
   return bufLength;
}


ssize_t device_write_custchar(struct file* filp, const char* bufSource,
   size_t bufLength, loff_t* curOffset) {
   printk(KERN_INFO DEVICE_NAME": writing to custchar %d\n", 
      virtual_device.cur_cust_char);
   lcdCustomCharUpload(virtual_device.cur_cust_char, bufSource);
   lcdCommand(LCDgoto_line1);
   virtual_device.lineFlag = 0;
   return bufLength;
}

ssize_t device_read_control(struct file* filp, char* bufStoreData,
   size_t bufLength, loff_t* curOffset) {
   printk(KERN_INFO DEVICE_NAME": reading from control operations\n");
   return 0;
}

ssize_t device_read_custchar(struct file* filp, char* bufStoreData,
   size_t bufLength, loff_t* curOffset){
   printk(KERN_INFO DEVICE_NAME": reading special character, printing to lcd\n");
   lcdWrite(virtual_device.cur_cust_char);
   return 0;
}

MODULE_LICENSE("GPL"); // module license: required to use some functionalities.
module_init(driver_entry); // declares which function runs on init.
module_exit(driver_exit);  // declares which function runs on exit.
