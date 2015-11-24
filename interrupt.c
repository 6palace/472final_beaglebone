/*
 * GPIO INTERRUPT
 *
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>                 // Required for the GPIO functions
#include <linux/interrupt.h>            // Required for the IRQ code

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A Button/LED test driver for the BBB");
MODULE_VERSION("0.1");

static unsigned int gpioButtons[] = {20, 75, 106};   ///< hard coding the button gpios
//75 points to empty gpio, since button1 shares output with pwm and can no longer be accessed
static unsigned int irqNumber[3];          ///< Used to share the IRQ number within this file
static unsigned int numberPresses = 0;  ///< For information, store the number of button presses

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
   return (irq_handler_t) IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}

/// This next calls are  mandatory -- they identify the initialization function
/// and the cleanup function (as above).
module_init(ebbgpio_init);
module_exit(ebbgpio_exit);