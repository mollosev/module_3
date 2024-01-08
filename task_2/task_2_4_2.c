#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char * argv[]) {
    int fd_fifo;
    int num;
    if ((fd_fifo = open("task_2_4.txt", O_RDONLY)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    while (read(fd_fifo, &num, sizeof(int)) > 0) {
        printf("%d ", num);
    }
    printf("\n");
    exit(EXIT_SUCCESS);
}
