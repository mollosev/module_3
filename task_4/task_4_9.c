#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#define SHM_NAME "/myshm"
#define PARENT_SEM_NAME "/parent_sem"
#define CHILD_SEM_NAME "/child_sem"
#define SHM_SIZE 300

void sigint();
void error(char *msg);

int is_end = 0;

int main(int argc, char **argv) {
    sem_unlink(PARENT_SEM_NAME);
    shm_unlink(SHM_NAME);
    sem_unlink(CHILD_SEM_NAME);
    int shm = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0600);
    if(shm == -1) error("SHM");
    if(ftruncate(shm, sizeof(int)*SHM_SIZE) == -1) error("TRUNC");
    sem_t *sem_parent = sem_open(PARENT_SEM_NAME, O_CREAT, 0600, 0);
    if(sem_parent == SEM_FAILED) error("SEM PARENT");
    sem_t *sem_child = sem_open(CHILD_SEM_NAME, O_CREAT, 0600, 1);
    if(sem_child == SEM_FAILED) error("SEM CHILD");
    int *ptr = (int*)mmap(NULL, sizeof(int)*300, PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
    if(ptr == (int*)-1) error("SHM ATTACH");
    int *start_shm = ptr;
    srand(time(NULL));
    signal(SIGINT, sigint);
    switch(fork()) {
        case 0: {
            sleep(1);
            while(1) {
                sem_wait(sem_parent);
                if(is_end) {
                    munmap(start_shm, sizeof(int)*SHM_SIZE);
                    exit(EXIT_SUCCESS);
                }
                int count = *ptr++;
                printf("COUNT = %d\n", count);
                int max=*ptr, min=*ptr;
                for(int i = 0; i < count; i++) {
                    printf("%d ", *ptr);
                    if(*ptr > max) max = *ptr;
                    else if(*ptr < min) min = *ptr;
                    ptr++;
                }
                printf("\n");
                *ptr++ = max;
                *ptr++ = min;
                sem_post(sem_child);
            }
            break;
        }
        default: {
            int count_of_sets = 0;
            while(1) {
                sem_wait(sem_child);
                if(start_shm != ptr) {
                    printf("MAX = %d\n", *ptr++);
                    printf("MIN = %d\n\n", *ptr++);
                    count_of_sets++;
                }
                sleep(3);
                if(is_end) {
                    printf("Количество обработанных наборов: %d\n", count_of_sets);
                    munmap(start_shm, sizeof(int)*SHM_SIZE);
                    sem_close(sem_child);
                    sem_close(sem_parent);
                    sem_unlink(CHILD_SEM_NAME);
                    sem_unlink(PARENT_SEM_NAME);
                    shm_unlink(SHM_NAME);
                    exit(EXIT_SUCCESS);
                }
                int count = rand() % 19 + 2;
                *ptr++ = count;
                for(int j = 0; j < count; j++) {
                    *ptr++ = rand() % 50;
                }
                sem_post(sem_parent);
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