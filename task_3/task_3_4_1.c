#include <mqueue.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFF_SIZE 100
#define MY_QUEUE "/my_queue"

int main(int argc, char * argv[]) {
    mq_unlink(MY_QUEUE);
    struct mq_attr queue_attr;
    queue_attr.mq_maxmsg = 256;
    queue_attr.mq_msgsize = BUFF_SIZE;
    mqd_t mq = mq_open(MY_QUEUE, O_CREAT | O_RDWR, 0600, &queue_attr);
    if(mq == (mqd_t)-1) {perror(NULL); exit(0);}
    char buff[BUFF_SIZE];
    int prio;
    FILE * file = fopen("queue.txt", "r");
    while(fgets(buff, BUFF_SIZE-1, file) != NULL) {
        if (mq_send(mq, buff, strlen(buff), 256) == -1) perror(NULL);
    }
    fclose(file);
    buff[0] = '\0'; 
    if (mq_send(mq, buff, 1, 255) < 0) perror(NULL);
    mq_close(mq);
    exit(EXIT_SUCCESS);
}
