//main.c

/////////// This should definitely be cleaned up :)
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <time.h>
#include <stropts.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <assert.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <sys/signalfd.h>
#include <unistd.h>
#include <string.h>

#define NUMPOLL 1

int main() {
	printf("Control Program\n");

	pid_t pid = fork(); 
	if (pid < 0) 
		printf("Couldn't launch process\n");
	else if (pid == 0) {
//MAIN PROGRAM

/* Interesting Links: https://gabrbedd.wordpress.com/2013/07/29/handling-signals-with-signalfd/ <- use signalfd for kernel module?
*/

struct pollfd pfd[NUMPOLL];
int ret;
ssize_t bytes;
int fd;
char* tok;

fd = open("/tmp/adcData", O_RDONLY);

pfd[0].fd = fd;
//pfd[0].events = POLLIN | POLLERR | POLLHUP;
pfd[0].events = POLLIN;

char databuf[1024] = "";

//TODO:Just take 12bit values and send them over the pipe then chop on 12 bit boundaries. No ascii conversion required

while(1) {
	printf("Waiting.\n");
	ret = poll(pfd, NUMPOLL, -1);
	read(fd, databuf, 1024);
	//printf("Received: %s", databuf);
	printf("Received:");
   tok = strtok(databuf, ",\n");
   int i = 0;
   while(i < 4 && tok != NULL) 
   {
   	i++;
      printf(" %s\n", tok);
      tok = strtok(NULL, ",\n");
   }

}

//END MAIN PROGRAM
	} else {
		pid_t pid2 = fork(); 
		if (pid2 < 0) 
			printf("Couldn't launch process\n");
		else if (pid2 == 0) {
			system ("./adc");
			printf("ADC\n");
		} else {
			system ("./motor");
			printf("MOTOR\n");
		}
	}
}