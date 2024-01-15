#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFF_SIZE 100
#define MY_QUEUE "/my_queue"

int main(int argc, char * argv[]) {
    mqd_t mq = mq_open(MY_QUEUE, O_RDWR);
    if (mq == -1) { perror(NULL); exit(0);}
    char buff[BUFF_SIZE];
    int prio;
    while(1) {
        while (mq_receive(mq, buff, BUFF_SIZE, &prio) < 0)  perror(NULL);
        if(prio == 255) { 
            mq_close(mq);
            mq_unlink(MY_QUEUE);
            break;
        }
        printf("Полученное сообщение: %s\n", buff);
    }
    exit(EXIT_SUCCESS);
}
