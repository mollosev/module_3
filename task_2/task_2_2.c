#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

void pipeChild(int *filedes, int *filedes2, char *argv[]);
void pipeParrent(int *filedes, int *filedes2, int rv, char *argv[]);

int main(int argc, char * argv[]) {
    int filedes[2], filedes2[2];
    pipe(filedes);
    pipe(filedes2);
    int rv;
    switch(fork()) {
        case -1:
            perror(NULL);
            exit(EXIT_FAILURE);
            break;
        case 0:
            pipeChild(filedes, filedes2, argv);
            break;
        default:
            pipeParrent(filedes, filedes2, rv, argv);
            break;
    }
    exit(EXIT_SUCCESS);
}

void pipeChild(int *filedes, int *filedes2, char *argv[]) {
    close(filedes[0]);
    close(filedes2[1]);
    int num;
    for (int i = 0; i < atoi(argv[1]); i++){
        num = rand() % 10;
        write(filedes[1], &num, sizeof(int));
    }
    sleep(1);
    printf("CHILD: \n");
    for (int i = 0; i < atoi(argv[1]); i++) {
        read(filedes2[0], &num, sizeof(int));
        printf("%d ", num);
    }
    printf("\n");
    close(filedes[1]);
    close(filedes2[0]);
    exit(EXIT_SUCCESS);        
}

void pipeParrent(int *filedes, int *filedes2, int rv, char *argv[]) {
    close(filedes[1]);
    close(filedes2[0]);
    int num;
    for (int i = 0; i < atoi(argv[1]); i++) {   
        read(filedes[0], &num, sizeof(int));
        printf("%d ", num);
        num *= 2;
        write(filedes2[1], &num, sizeof(int));
    }
    printf("\n");
    wait(&rv);
    close(filedes[0]);
    close(filedes2[1]);
    exit(EXIT_SUCCESS);
}