#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>

void lock(int signal);
void unlock();

static int is_blocked = 1;

int main(int argc, char * argv[]) {
    if(argc < 2 || atoi(argv[1]) == 0) {
        printf("Неправильные аргументы\n");
        exit(EXIT_FAILURE);
    }
    int filedes[2];
    pipe(filedes);
    int pid;
    switch (pid = fork()) {
        case -1:
            perror(NULL);
            exit(EXIT_FAILURE);
            break;
        case 0: {
            signal(SIGUSR1, lock);
            signal(SIGUSR2, lock);
            close(filedes[0]);
            int num;
            int file;
            for (int i=0; i<atoi(argv[1]); i++) {
                num = rand() % 10;
                if(write(filedes[1], &num, sizeof(int)) == -1)
                    perror("Запись в канал");
                sleep(1);
                while(is_blocked == 1) sleep(1);
                file = open("task_2_8.bin", O_RDONLY);
                if(file == -1)
                    perror("Не открыт файл");
                if(read(file, &num, sizeof(int)) == -1)
                    perror("Чтение из файла");
                printf("Число из файла: %d\n", num);
                close(file);
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
            unlink("task_2_8.bin");
            for(int i=0; i<atoi(argv[1]); i++) {
                while(read(filedes[0], &num, sizeof(int)) == 0);
                kill(pid, SIGUSR1);
                sleep(1);
                printf("Число от дочернего: %d\n", num);
                file = open("task_2_8.bin", O_WRONLY | O_CREAT, 0600);
                if(write(file, &num, sizeof(int)) == -1)
                    perror("Запись в файл");
                close(file);
                kill(pid, SIGUSR2);
            }
            close(filedes[0]);
            break;
        }
    }
    exit(EXIT_SUCCESS);
}

void lock(int signal) {
    if(signal == SIGUSR1)
        is_blocked = 1;
    else if(signal == SIGUSR2)
        is_blocked = 0;
}

void unlock() {
    is_blocked = 0;
}