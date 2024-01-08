#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_SIZE 10

int main(int argc, char * argv[]) {
    int fd_fifo;
    int num;
    int count;
    if(argc < 2)
        count = 10;
    else 
        count = atoi(argv[1]);
    unlink("task_4_4.txt");
    if((mkfifo("task_4_4.txt", 0600)) == -1) {
        perror("Make fifo");
        exit(EXIT_FAILURE);
    }
    if((fd_fifo = open("task_4_4.txt", O_WRONLY)) == -1) {
        perror("Open fifo");
        exit(EXIT_FAILURE);
    }
    sem_unlink("task_4_1_mutex.txt");
    sem_unlink("task_4_1_empty.txt");
    sem_unlink("task_4_1_full.txt");
    sem_t *sem_push = sem_open("task_4_1_mutex.txt", O_CREAT, 0600, 1);
    if(sem_push == SEM_FAILED) perror("push");
    sem_t *sem_empty = sem_open("task_4_1_empty.txt", O_CREAT, 0600, FIFO_SIZE);
    if(sem_empty == SEM_FAILED) perror("empty");
    sem_t *sem_full = sem_open("task_4_1_full.txt", O_CREAT, 0600, 0);
    if(sem_full == SEM_FAILED) perror("full");
    int sem_full_value;
    for (int i = 0; i < count; i++) {
        for(sem_getvalue(sem_full, &sem_full_value); sem_full_value != 0; sem_getvalue(sem_full, &sem_full_value));
        sem_wait(sem_push);
        for (int j = 0; j < 10; j++) {
            num = rand() % 100;
            write(fd_fifo, &num, sizeof(int));
            sem_wait(sem_empty);
            sem_post(sem_full);
        }
        sem_post(sem_push);
    }
    close(fd_fifo);
    sem_close(sem_push);
    sem_close(sem_empty);
    sem_close(sem_full);
    exit(EXIT_SUCCESS);
}
