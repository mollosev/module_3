#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/stat.h>

int main(int argc, char * argv[]) {
    if(argc < 2 || atoi(argv[1]) == 0) {
        printf("Неправильные аргументы\n");
        exit(EXIT_FAILURE);
    }
    sem_unlink("/not_empty");
    sem_unlink("/sigusr");
    sem_t* sem = sem_open("/sigusr", O_CREAT, 0600, 1);
    if(sem == SEM_FAILED) perror("Semaphore create");
    sem_t* not_empty = sem_open("/not_empty", O_CREAT, 0600, 0);
    if(not_empty == SEM_FAILED) perror("Sem not empty");
    int filedes[2];
    pipe(filedes);
    int pid;
    switch (pid = fork()) {
        case -1:
            perror(NULL);
            exit(EXIT_FAILURE);
            break;
        case 0: {
            close(filedes[0]);
            int num;
            int file;
            int value_sem;
            for(int i=0; i < atoi(argv[1]); i++) {
                num = rand() % 10;
                if(write(filedes[1], &num, sizeof(int)) == -1)
                    perror("Запись в канал");
                sem_wait(not_empty);
                sem_wait(sem);
                file = open("task_4_5.bin", O_RDONLY);
                if(file == -1)
                    perror("Не открыт файл");
                if(read(file, &num, sizeof(int)) == -1)
                    perror("Чтение из файла");
                printf("Число из файла: %d\n", num);
                close(file);
                sem_post(sem);
            }
            close(filedes[1]);
            sem_close(sem);
            sem_close(not_empty);
            exit(EXIT_SUCCESS);
            break;
        }
        default: {
            close(filedes[1]);
            int num;
            int file;
            int rd;
            int value_sem;
            unlink("task_4_5.bin");
            for(int i=0; i < atoi(argv[1]); i++) {
                while(read(filedes[0], &num, sizeof(int)) == 0);
                printf("Число от дочернего: %d\n", num);
                sem_wait(sem);
                file = open("task_4_5.bin", O_WRONLY | O_CREAT, 0600);
                if(write(file, &num, sizeof(int)) == -1)
                    perror("Запись в файл");
                close(file);
                sem_post(not_empty);
                sem_post(sem);
            }
            close(filedes[0]);
            break;
        }
    }
    sem_close(sem);
    sem_close(not_empty);
    sem_unlink("/not_empty");
    sem_unlink("/sigusr");
    exit(EXIT_SUCCESS);
}
