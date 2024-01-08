#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char * argv[]) {
    int fd_fifo;
    int num;
    unlink("task_2_4.txt");
    if((mkfifo("task_2_4.txt", 0700)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    if((fd_fifo = open("task_2_4.txt", O_WRONLY)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < atoi(argv[1]); i++) {
        num = rand() % 100;
        write(fd_fifo, &num, sizeof(int));
    }
    exit(EXIT_SUCCESS);
}
