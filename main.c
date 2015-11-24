//main.c
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h>
#include <time.h>

int main() {
	printf("Control Program\n");

	pid_t pid = fork(); 
	if (pid < 0) 
		printf("Couldn't launch process\n");
	else if (pid == 0) {
		while(1) {
			printf("MAIN!\n");

			char charBuf[10000];

			FILE* adc4 = fopen("/tmp/adcData4", "r");
			fscanf(adc4, "%s", charBuf);
			printf("%s",charBuf);

			sleep(1);
		}
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