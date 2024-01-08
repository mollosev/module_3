#include <sys/msg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define BUFF_SIZE 100
struct msgbuf {
    long mtype;
    char mtext[1];
};

int main(int argc, char * argv[]) {
    key_t key = ftok("task_3_1", 98);
    int msg = msgget(key, 0666);
    char buff[BUFF_SIZE];
    struct msgbuf mbuff;
    while(1) {
        while(msgrcv(msg, &mbuff, BUFF_SIZE, 0, 0) < 0) perror(NULL);
        if(mbuff.mtype == 255) { 
        mbuff.mtype = 256;
            if(msgsnd(msg, &mbuff, 1, 0) <= 0) perror(NULL);
        break;}
        printf("Полученное сообщение: %s %ld\n", mbuff.mtext, mbuff.mtype);
    }
    exit(EXIT_SUCCESS);
}
