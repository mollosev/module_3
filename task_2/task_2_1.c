#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

void pipeChild(int *filedes, char *argv[]);
void pipeParrent(int *filedes);

int main(int argc, char * argv[]) {
    int filedes[2];
    pipe(filedes);
    switch(fork()) {
        case -1:
            perror(NULL);
            exit(EXIT_FAILURE);
            break;
        case 0:
            pipeChild(filedes, argv);
            break;
        default:
            pipeParrent(filedes);
            break;
    }
    exit(EXIT_SUCCESS);
}

void pipeChild(int *filedes, char *argv[]) {
    close(filedes[0]);
    int num;
    for (int i = 0; i < atoi(argv[1]); i++) {
        num = rand() % 10;
        write(filedes[1], &num, sizeof(int));
    }
    close(filedes[1]);
    exit(EXIT_SUCCESS);            
}

void pipeParrent(int *filedes) {
    close(filedes[1]);
    int num;
    int file = open("task_2_1.bin", O_RDWR | O_TRUNC | O_CREAT, 0777);
    while (read(filedes[0], &num, sizeof(int)) > 0) {
        write(file, &num, sizeof(int));
        printf("%d ", num);
    }
    printf("\n");
    close(file);
    close(filedes[0]);
}