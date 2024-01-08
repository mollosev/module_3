#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

void sigint();

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

int is_end = 0;

int main(int argc, char **argv) {
    int shm = shmget(IPC_PRIVATE, sizeof(int)*500, IPC_CREAT | IPC_EXCL | 0600);
    if(shm == -1) { perror("SHM"); exit(EXIT_FAILURE); }
    int sem = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    if(sem == -1) { perror("SEM"); exit(EXIT_FAILURE); }
    struct sembuf lock[2] = {{0, 0, 0}, {0, 1, 0}};
    struct sembuf unlock = {0, -1, 0};
    int *ptr = (int*)shmat(shm, NULL, 0);
    if(ptr == (int*)-1) { perror("SHM ATTACH"); exit(EXIT_FAILURE); }
    int *start_shm = ptr;
    srand(time(NULL));
    signal(SIGINT, sigint);
    switch(fork()) {
        case 0: {
            sleep(1);
            while(1) {
                semop(sem, lock, 2);
                if(is_end) {
                    shmdt(start_shm);
                    break;
                }
                int count = *ptr++;
                printf("COUNT = %d\n", count);
                int max=*ptr, min=*ptr;
                for(int i=0; i < count; i++) {
                    printf("%d ", *ptr);
                    if(*ptr > max) max = *ptr;
                    else if(*ptr < min) min = *ptr;
                    ptr++;
                }
                printf("\n");
                *ptr++ = max;
                *ptr++ = min;
                semop(sem, &unlock, 1);
            }
            break;
        }
        default: {
            int count_of_sets = 0;
            while(1) {
                semop(sem, lock, 2);
                if(start_shm != ptr) {
                    printf("MAX = %d\n", *ptr++);
                    printf("MIN = %d\n", *ptr++);
                    count_of_sets++;
                }
                sleep(3);
                if(is_end) {
                    printf("Количество обработанных наборов: %d\n", count_of_sets);
                    shmdt(start_shm);
                    shmctl(shm, IPC_RMID, NULL);
                    semctl(sem, 0, IPC_RMID);
                    exit(EXIT_SUCCESS);
                }
                int count = rand() % 19 + 2;
                *ptr++ = count;
                for(int j = 0; j < count; j++) {
                    *ptr++ = rand() % 50;
                }
                semop(sem, &unlock, 1);
            }
            break;
        }
    }
    exit(EXIT_SUCCESS);
}

void sigint() {
    is_end = 1;
}