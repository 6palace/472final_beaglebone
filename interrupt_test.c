#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>       
#include <sys/types.h>
#include <unistd.h>       

#define SIG_TEST 44 /* we define our own signal, hard coded since SIGRTMIN is different in user and in kernel space */ 
void receiveData(int n, siginfo_t *info, void *unused);

int main(){
	int configfd;
	char buf[10];

	struct sigaction sig;
	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIG_TEST, &sig, NULL);

	configfd = open("/dev/buttons_char/proc", O_WRONLY);
	if(configfd < 0) {
		printf("couldn't open buttons_char\n");
		return -1;
	}
	sprintf(buf, "%i", getpid());
	if (write(configfd, buf, strlen(buf) + 1) < 0) {
		printf("couldn't fwrite to button_char\n"); 
		return -1;
	}
}


void receiveData(int n, siginfo_t *info, void *unused) {
	printf("received value %i\n", info->si_int);
}