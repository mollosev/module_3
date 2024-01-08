#include <sys/msg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFF_SIZE 100
struct msgbuf {
    long mtype;
    char mtext[1];
};

int main(int argc, char * argv[]) {
    key_t key = ftok("task_3_1.txt", 98);
    int msg = msgget(key, IPC_CREAT | 0666);
    if(msg == -1) perror(NULL);
    char buff[BUFF_SIZE];
    struct msgbuf mbuff;
    FILE * file = fopen("task_3_1.txt", "r");
    mbuff.mtype = 1;
    while(fgets(mbuff.mtext, BUFF_SIZE, file) != NULL) {
        if (msgsnd(msg, &mbuff, BUFF_SIZE, 0) == -1) perror(NULL);
    }
    fclose(file);
    mbuff.mtype = 255;
    mbuff.mtext[0] = '\0';
    if (msgsnd(msg, &mbuff, 1, 0) < 0) perror(NULL);
    while (msgrcv(msg, &mbuff, BUFF_SIZE, 256, 0) == 0) perror(NULL);
    msgctl(msg, IPC_RMID, NULL);
    exit(EXIT_SUCCESS);
}
