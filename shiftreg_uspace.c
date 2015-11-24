#include <stdio.h>
#include <unistd.h>

int main() {

	FILE* sreg = fopen("/dev/sreg", "w");
	for(int i = 0; i <= 255; i++ ) {
		fprintf(sreg, "%c", i);
		fflush(sreg);
		usleep(1);
	}
	fclose(sreg);
}