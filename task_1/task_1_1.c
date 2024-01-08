#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void exitFunc(void);

int main(int argc, char *argv[]) {
	pid_t pid;
	int state = atexit(exitFunc);
	if (state != 0) {
		fprintf(stderr, "CANNOT SET EXITFUNC!\n");
		exit(EXIT_FAILURE);
	}
	switch(pid = fork()){
		case -1:
			printf("ERROR\n");
			exit(EXIT_FAILURE);
            break;
		case 0:
			printf("CHILD(%d):\n", getpid());
			for (int i = 1; i < argc; i++)
				printf("%d: %s\n", i, argv[i]);
			exit(EXIT_SUCCESS);
            break;
		default:
			printf("PARENT(%d):\n", getpid());
			for (int i = 1; i < argc; i++)
				printf("%d: %s\n", i, argv[i]);
			exit(EXIT_SUCCESS);
            break;
	}
}

void exitFunc(void) {
	printf("GOOD BYE\n");
}