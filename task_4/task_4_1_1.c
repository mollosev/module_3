#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define FIFO_SIZE 10

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main(int argc, char * argv[]) {
    int fd_fifo;
    int num;
    int count;
    if(argc < 2)
        count = 10;
    else 
        count = atoi(argv[1]);
    unlink("task_4_1.txt");
    if((mkfifo("task_4_1.txt", 0600)) == -1) {
        perror("Make fifo");
        exit(EXIT_FAILURE);
    }
    if((fd_fifo = open("task_4_1.txt", O_WRONLY, O_NONBLOCK)) == -1) {
        perror("Open fifo");
        exit(EXIT_FAILURE);
    }
    key_t key = ftok("task_4_1.txt", 100);
    int sem = semget(key, 3, IPC_CREAT | 0600);
    if(sem == -1) perror(NULL);
    union semun arg;
    unsigned short values[3] = {1, FIFO_SIZE, 0};
    arg.array = values;
    semctl(sem, 0, SETALL, arg);
    struct sembuf push[2] = {{2, 0, 0}, {0, -1, 0}};
    struct sembuf add[2] = {{1, -1, IPC_NOWAIT}, {2, 1, 0}};
    struct sembuf unlock = {0, 1, 0};
    for (int i = 0; i<count; i++) {
        semop(sem, push, 2);
        for(int j = 0; j < 10; j++) {
            num = rand() % 100;
            write(fd_fifo, &num, sizeof(int));
            semop(sem, add, 2);
        }
        semop(sem, &unlock, 1);
    }
    exit(EXIT_SUCCESS);
}
