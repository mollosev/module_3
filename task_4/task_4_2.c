#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/sem.h>

int main(int argc, char * argv[]) {
    if(argc < 2 || atoi(argv[1]) == 0) {
        printf("Неправильные аргументы\n");
        exit(EXIT_FAILURE);
    }
    int sem = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if(sem == -1) perror("Semaphore create");
    struct sembuf lock[2] = {{0,0,0}, {0,1,0}};
    struct sembuf unlock = {0,-1,0};
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
            for(int i=0; i < atoi(argv[1]); i++) {
                num = rand() % 10;
                if(write(filedes[1], &num, sizeof(int)) == -1)
                    perror("Запись в канал");
                semop(sem, lock, 2);
                file = open("task_4_2.bin", O_RDONLY);
                if(file == -1)
                    perror("Не открыт файл");
                if(read(file, &num, sizeof(int)) == -1)
                    perror("Чтение из файла");
                printf("Число из файла: %d\n", num);
                close(file);
                semop(sem, &unlock, 1);
            }
            close(filedes[1]);
            exit(EXIT_SUCCESS);
            break;
        }
        default: {
            close(filedes[1]);
            int num;
            int file;
            int rd;
            unlink("task_4_2.bin");
            for(int i = 0; i < atoi(argv[1]); i++) {
                semop(sem, lock, 2);
                while(read(filedes[0], &num, sizeof(int)) == 0);
                printf("Число от дочернего: %d\n", num);
                file = open("task_4_2.bin", O_WRONLY | O_CREAT, 0600);
                if(write(file, &num, sizeof(int)) == -1)
                    perror("Запись в файл");
                close(file);
                semop(sem, &unlock, 1);
            }
            close(filedes[0]);
            break;
        }
    }
    semctl(sem, 0, IPC_RMID);
    exit(EXIT_SUCCESS);
}
