#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>
#include <semaphore.h>

#define MAX_READERS 5
#define SEM_ALLOW_READ "/sem_allow"
#define SEM_BLOCK_READ "/sem_block"
#define SEM_MAIN_CHILD "/sem_child"

void error(char * msg);
void unlinkSems();

int main(int argc, char * argv[]) {
    if(argc < 2 || atoi(argv[1]) == 0) {
        printf("Неправильные аргументы\n");
        exit(EXIT_FAILURE);
    }
    unlinkSems();
    sem_t *sem_allow_read, *sem_block_read, *sem_main_child;
    if((sem_allow_read = sem_open(SEM_ALLOW_READ, O_CREAT, 0600, 0)) == SEM_FAILED) error("SEM ALOOW");
    if((sem_block_read = sem_open(SEM_BLOCK_READ, O_CREAT, 0600, 5)) == SEM_FAILED) error("SEM BLOCK");
    if((sem_main_child = sem_open(SEM_MAIN_CHILD, O_CREAT, 0600, 0)) == SEM_FAILED) error("SEM CHILD");
    int filedes[2];
    pipe(filedes);
    int pid;
    struct timespec timer;
    switch (pid = fork()) {
        case -1:
            perror(NULL);
            exit(EXIT_FAILURE);
            break;
        case 0: {
            printf("Создался %d\n", getpid());
            close(filedes[0]);
            int num;
            int file;
            for(int i=0; i < atoi(argv[1]); i++) {
                num = rand() % 10;
                if(write(filedes[1], &num, sizeof(int)) == -1)
                    perror("Запись в канал");
                clock_gettime(0, &timer);
                timer.tv_sec += 10;
                if(sem_timedwait(sem_main_child, &timer) == -1) {
                    break;
                }
                file = open("task_4_6.bin", O_RDONLY);
                if(file == -1)
                    perror("Не открыт файл");
                if(read(file, &num, sizeof(int)) == -1)
                    perror("Чтение из файла");
                printf("Число из файла %d: %d\n", getpid(), num);
                close(file);
                sem_post(sem_block_read);
            }
            close(filedes[1]);
            exit(EXIT_SUCCESS);
            break;
        }
        default: {
            int num;
            int file;
            int rd;
            unlink("task_4_6.bin");
            file = open("task_4_6.bin", O_WRONLY | O_CREAT, 0600);
            close(file);
            close(filedes[1]);
            for(int i = 0; i < (int)MAX_READERS/2; i++) {
                switch(fork()) {
                    case 0: {
                        printf("Создался %d\n", getpid());
                        close(filedes[0]);
                        while(1) {
                            int num, file;
                            clock_gettime(0, &timer);
                            timer.tv_sec += 10;
                            if(sem_timedwait(sem_allow_read, &timer) == -1) {
                                break;
                            }
                            file = open("task_4_6.bin", O_RDONLY);
                            if(file == -1)
                                perror("Не открыт файл");
                            if(read(file, &num, sizeof(int)) == -1)
                                perror("Чтение из файла");
                            printf("Число из файла %d: %d\n", getpid(), num);
                            close(file);
                            sem_post(sem_block_read);
                        }
                        exit(EXIT_SUCCESS);
                        break;
                    }
                    default:
                        break;
                }
            }
            int sem_block_value;
            for(int i=0; i < atoi(argv[1]); i++) {
                for(sem_getvalue(sem_block_read, &sem_block_value); sem_block_value != MAX_READERS; sem_getvalue(sem_block_read, &sem_block_value));
                for(int i=0; i < MAX_READERS; i++) sem_wait(sem_block_read);
                while(read(filedes[0], &num, sizeof(int)) == 0);
                printf("Число от дочернего: %d\n", num);
                file = open("task_4_6.bin", O_WRONLY | O_CREAT, 0600);
                if(write(file, &num, sizeof(int)) == -1)
                    perror("Запись в файл");
                close(file);
                sem_post(sem_main_child);
                for(int i=0; i < MAX_READERS-1; i++) sem_post(sem_allow_read);
            }
            close(filedes[0]);
            sleep(1);
            sem_close(sem_allow_read);
            sem_close(sem_block_read);
            sem_close(sem_main_child);
            unlinkSems();
            break;
        }
    }
    exit(EXIT_SUCCESS);
}

void error(char * msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void unlinkSems() {
    sem_unlink(SEM_ALLOW_READ);
    sem_unlink(SEM_BLOCK_READ);
    sem_unlink(SEM_MAIN_CHILD);
}