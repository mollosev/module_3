#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>
#include <time.h>

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main(int argc, char * argv[]) {
    int fd_fifo;
    int num;
    if ((fd_fifo = open("task_4_1.txt", O_RDONLY | O_NONBLOCK)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    sleep(1);
    key_t key = ftok("task_4_1.txt", 100);
    int sem = semget(key, 3, 0600);
    if(sem == -1)  {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    struct sembuf pop[2] = {{1, 0, 0}, {0, -1, 0}};
    struct sembuf delete[2] = {{1, 1, 0}, {2, -1, IPC_NOWAIT}};
    struct sembuf unlock = {0, 1, 0};
    struct timespec time = {5, 0};
    while(1){
        if(semtimedop(sem, pop, 2, &time) == -1) break;
        while(read(fd_fifo, &num, sizeof(int)) > 0) {
            printf("%d ", num);
            semop(sem, delete, 2);
        }
        printf("\n");
        semop(sem, &unlock, 1);
    }
    semctl(sem, 0, IPC_RMID);
    exit(EXIT_SUCCESS);
}
