#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include <errno.h>
#include <time.h>

#define SHM_NAME "/myshm"
#define PARENT_SEM_NAME "/parent_sem"
#define CHILD_SEM_NAME "/child_sem"
#define SHM_SIZE 300

void sigint();
void error(char *msg);
void waitParent(int** ptr);
void childPref(sem_t *sem, int* start_shm, struct timespec* timer);

int is_end = 0;

int main(int argc, char **argv) {
    sem_unlink(PARENT_SEM_NAME);
    shm_unlink(SHM_NAME);
    sem_unlink(CHILD_SEM_NAME);
    int shm = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if(shm == -1) error("SHM");
    if(ftruncate(shm, sizeof(int)*SHM_SIZE) == -1) error("TRUNC");
    sem_t *sem = sem_open(CHILD_SEM_NAME, O_CREAT, 0600, 1);
    if(sem == SEM_FAILED) error("SEM CHILD");
    int *ptr = (int*)mmap(NULL, sizeof(int)*300, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if(ptr == (int*)-1) error("SHM ATTACH");
    int *start_shm = ptr;
    *ptr = -1;
    srand(time(NULL));
    struct timespec timer;
    switch(fork()) {
        case 0: {
            waitParent(&ptr);
            while(1) {
                childPref(sem, start_shm, &timer);
                int count = *ptr++;
                int max=*ptr, min=*ptr;
                for(int i = 0; i < count; i++) {
                    if(*ptr > max) max = *ptr;
                    else if(*ptr < min) min = *ptr;
                    ptr++;
                }
                *ptr++ = max;
                *ptr = min;
                ptr += 4;
                sem_post(sem);
                sleep(2);
            }
            break;
        }
        default: {
            switch(fork()) {
                case 0:
                    waitParent(&ptr);
                    while(1) {
                        childPref(sem, start_shm, &timer);
                        int count = *ptr++;
                        int sum = 0;
                        for(int i = 0; i < count; i++) {
                            sum += *ptr++ % 15;
                        }
                        *(ptr+2) = sum;
                        ptr += 5;
                        sem_post(sem);
                        sleep(2);
                    }
                    break;
                default:
                    switch(fork()) {
                        case 0:
                            waitParent(&ptr);
                            while(1) {
                                childPref(sem, start_shm, &timer);
                                int count = *ptr++;
                                int sum = 0;
                                for(int i = 0; i < count; i++) {
                                    sum += *ptr++;
                                }
                                *(ptr+3) = sum;
                                ptr += 5;
                                sem_post(sem);
                                sleep(2);
                            }
                            break;
                        default:
                            switch(fork()) {
                                case 0:
                                    waitParent(&ptr);
                                    while(1) {
                                        childPref(sem, start_shm, &timer);
                                        int count = *ptr++;
                                        int sum = 0;
                                        for(int i=0; i<count; i++) {
                                            sum += *ptr++;
                                        }
                                        *(ptr+4) = (int)(sum/count);
                                        ptr += 5;
                                        sem_post(sem);
                                        sleep(2);
                                    }
                                    break;
                                default:
                                    break;
                            }
                            break;
                    }
                    break;
            }
            int count_of_sets = 0;
            signal(SIGINT, sigint);
            *ptr++ = 1;
            while(1) {
                while(sem_wait(sem) != 0); 
                if(start_shm+1 != ptr) {
                    printf("MAX = %d\n", *ptr++);
                    printf("MIN = %d\n", *ptr++);
                    printf("SUM_REST15 = %d\n", *ptr++);
                    printf("SUM = %d\n", *ptr++);
                    printf("AVG = %d\n\n", *ptr++);
                    count_of_sets++;
                }
                sleep(3);
                if(is_end) {
                    printf("Количество обработанных наборов: %d\n", count_of_sets);
                    munmap(start_shm, sizeof(int)*SHM_SIZE);
                    sem_close(sem);
                    sem_unlink(CHILD_SEM_NAME);
                    sem_unlink(PARENT_SEM_NAME);
                    shm_unlink(SHM_NAME);
                    exit(EXIT_SUCCESS);
                }
                int count = rand() % 19 + 2;
                *ptr++ = count;
                printf("COUNT = %d\n", count);
                for(int j = 0; j < count; j++) {
                    *ptr = rand() % 20;
                    printf("%d ", *ptr++);
                }
                printf("\n");
                sem_post(sem);
                sleep(1);
            }
            break;
        }
    }
    exit(EXIT_SUCCESS);
}

void sigint() {
    is_end = 1;
}

void error(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

void waitParent(int** ptr) {
    while(**ptr != 1) sleep(1);
    *ptr += 1;
}

void childPref(sem_t *sem, int* start_shm, struct timespec* timer) {
    clock_gettime(0, timer);
    timer->tv_sec += 10;
    if(sem_timedwait(sem, timer) == -1) {
        sem_close(sem);
        munmap(start_shm, sizeof(int)*SHM_SIZE);
        exit(EXIT_SUCCESS);
    }
}