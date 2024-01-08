#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <time.h>

#define MAX_READERS 5

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int main(int argc, char * argv[]) {
    if(argc < 2 || atoi(argv[1]) == 0) {
        printf("Неправильные аргументы\n");
        exit(EXIT_FAILURE);
    }
    int sem = semget(IPC_PRIVATE, 3, IPC_CREAT | 0600); 
    if(sem == -1) perror("Semaphore create");
    struct sembuf start_read = {0, -1, 0};
    struct sembuf end_read = {1, 1, 0};
    struct sembuf start_write = {1, -MAX_READERS, 0};
    struct sembuf end_write = {0, MAX_READERS, 0};
    union semun arg;
    arg.val = 1;
    semctl(sem, 2, SETVAL, arg);
    arg.val = MAX_READERS;
    semctl(sem, 1 ,SETVAL, arg);
    struct sembuf wait_start = {2, 0, 0};
    struct sembuf start_prog = {2, -1, 0};
    int filedes[2];
    pipe(filedes);
    int pid;
    struct timespec time = {5, 0};
    switch (pid = fork()) {
        case -1:
            perror(NULL);
            exit(EXIT_FAILURE);
            break;
        case 0: {
            printf("Создался %d\n", getpid());
            semop(sem, &wait_start, 1);
            close(filedes[0]);
            int num;
            int file;
            for(int i=0; i < atoi(argv[1]); i++) {
                num = rand() % 10;
                if(write(filedes[1], &num, sizeof(int)) == -1)
                    perror("Запись в канал");
                if(semtimedop(sem, &start_read, 1, &time) == -1) {
                    break;
                }
                file = open("task_4_3.bin", O_RDONLY);
                if(file == -1)
                    perror("Не открыт файл");
                if(read(file, &num, sizeof(int)) == -1)
                    perror("Чтение из файла");
                printf("Число из файла %d: %d\n", getpid(), num);
                close(file);
                semop(sem, &end_read, 1);
            }
            close(filedes[1]);
            exit(EXIT_SUCCESS);
            break;
        }
        default: {
            int num;
            int file;
            int rd;
            unlink("task_4_3.bin");
            file = open("task_4_3.bin", O_WRONLY | O_CREAT, 0600);
            close(file);
            close(filedes[1]);
            for(int i = 0; i < (int)MAX_READERS/2; i++) {
                switch(fork()) {
                    case 0: {
                        printf("Создался %d\n", getpid());
                        close(filedes[0]);
                        while(1) {
                            int num, file;
                            if(semtimedop(sem, &start_read, 1, &time) == -1) {
                                break;
                            }
                            file = open("task_4_3.bin", O_RDONLY);
                            if(file == -1)
                                perror("Не открыт файл");
                            if(read(file, &num, sizeof(int)) == -1)
                                perror("Чтение из файла");
                            printf("Число из файла %d: %d\n", getpid(), num);
                            close(file);
                            semop(sem, &end_read, 1);
                        }
                        printf("Удалился %d\n", getpid());
                        exit(EXIT_SUCCESS);
                        break;
                    }
                    default:
                        break;
                }
            }
            semop(sem, &start_prog, 1);
            for(int i=0; i < atoi(argv[1]); i++) {
                semop(sem, &start_write, 1);
                while (read(filedes[0], &num, sizeof(int)) == 0);
                printf("Число от дочернего: %d\n", num);
                file = open("task_4_3.bin", O_WRONLY | O_CREAT, 0600);
                if (write(file, &num, sizeof(int)) == -1)
                    perror("Запись в файл");
                close(file);
                semop(sem, &end_write, 1);
            }
            close(filedes[0]);
            sleep(1);
            semctl(sem, 0, IPC_RMID);
            break;
        }
    }
    exit(EXIT_SUCCESS);
}
