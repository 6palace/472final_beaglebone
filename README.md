This Lab comprises several units working together to create a car that can travel and react to it's environment, using both LKMs and user-space processes scommunicating with each other. There are 4 main parts:

	1. Button interrupt driver: Instead of polling button GPIOs, our new button LKM sends an interrupt whenever a button is pressed, displays the current button status when it's character device file is read.

	2. ADC : This user-space program sets up and reads the analog information from four distance sensors coming into the beaglebone's adc inputs. It processes and displays this data as integer values in a named pipe at '/tmp/adcData'. This operation is done on a timer interrupt and runs in the background, constantly updating the output of the pipe.

	3. Motor : this process sets up a FIFO pipe at '/tmp/motorData,' where it listens for instructions from other processes. Based on those instructions, it uses pwm outputs to control the board's treads.

	4. Control: This process serves as a top-level program. It  initializes ADC and Motor as child processes, and links itself to their pipes, as well as accessing the character device of the button interrupt driver. Using this, it controls the behavior of the cart. The current behavior is to go straight until it readches an obstacle, upon which it turns right on the spot until an open path is found. The control header file also provides structures to easily control the cart's behavior. 
