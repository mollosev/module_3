#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	pid_t pid;
	int tmp;
	switch(pid = fork()) {
		case -1:
			printf("ERROR\n");
			exit(EXIT_FAILURE);
		case 0:
			printf("CHILD(%d):\n", getpid());
			for (int i = argc / 2 + 1; i < argc; i++) {
				tmp = atoi(argv[i]);
				printf("%d: SIDE:%d SQUARE=%d\n", i, tmp, tmp * tmp);
			}
			exit(EXIT_SUCCESS);
		default:
			printf("PARENT(%d):\n", getpid());
			for (int i = 1; i <= argc / 2; i++) {
				tmp = atoi(argv[i]);
				printf("%d: SIDE=%d SQUARE=%d\n", i, tmp, tmp * tmp);
			}
			exit(EXIT_SUCCESS);
	}
}