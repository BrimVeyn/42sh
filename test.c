
#include "signal.h"
#include <stdlib.h>
#include <stdio.h>


int main(void) {
	printf("SIGINT = %d\n", SIGINT);
	printf("SIGTERM = %d\n", SIGTERM);
	printf("SIGQUIT = %d\n", SIGQUIT);
}
