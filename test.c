#define _GNU_SOURCE 1
#include <unistd.h>

int main(void) {
	int pipefd[2];
	pipe2(pipefd, NULL);
}
