#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char * argv[]) {
    int fd_fifo;
    int num;
    if((fd_fifo = open("task_4_4.txt", O_RDONLY | O_NONBLOCK)) == -1) {
        perror(NULL);
        exit(EXIT_FAILURE);
    }
    sleep(1);
    sem_t *sem_pop = sem_open("task_4_1_mutex.txt", 0);
    if(sem_pop == SEM_FAILED) perror("pop");
    sem_t *sem_empty = sem_open("task_4_1_empty.txt", 0);
    if(sem_empty == SEM_FAILED) perror("empty");
    sem_t *sem_full = sem_open("task_4_1_full.txt", 0);
    if(sem_full == SEM_FAILED) perror("full");
    int sem_empty_value;
    int prev_val;
    while(1) {
        prev_val = -1;
        for(sem_getvalue(sem_empty, &sem_empty_value); sem_empty_value != 0; sem_getvalue(sem_empty, &sem_empty_value)) {
            if(prev_val == sem_empty_value) {
                close(fd_fifo);
                sem_close(sem_pop);
                sem_close(sem_empty);
                sem_close(sem_full);
                sem_unlink("task_4_1_mutex.txt");
                sem_unlink("task_4_1_empty.txt");
                sem_unlink("task_4_1_full.txt");
                exit(EXIT_SUCCESS);
            } 
            prev_val = sem_empty_value; 
            sleep(1); 
        }
        sem_wait(sem_pop);
        while(read(fd_fifo, &num, sizeof(int)) > 0) {
            printf("%d ", num);
            sem_wait(sem_full);
            sem_post(sem_empty);
        }
        sem_post(sem_pop);
        printf("\n");
    }
    exit(EXIT_SUCCESS);
}
