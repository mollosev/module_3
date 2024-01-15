#include <sys/msg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

#define BUFF_SIZE 200
#define MSG_EXCEPT 020000
#define MAX_COUNT_CLIENTS 10



struct msgbuf {
    long mtype;
    struct Text {
        int id;
        char message[BUFF_SIZE+50];
    } mtext;
};

int main(int argc, char * argv[])
{
    key_t key = ftok("server.txt", 95);
    int msg = msgget(key, IPC_CREAT | 0666);
    char buff[BUFF_SIZE];
    struct msgbuf mbuff;
    int clients[MAX_COUNT_CLIENTS];
    int count_of_clients = 0, no_clients = 0;

    int id_sender;
    char message[BUFF_SIZE];
    while(1)
    {
        mbuff.mtype = 0;
        if (no_clients)
        {
            msgctl(msg, IPC_RMID, NULL);
            exit(EXIT_SUCCESS);
        }
        if (msgrcv(msg, &mbuff, sizeof(struct Text), 1, IPC_NOWAIT) > 0)
        {
            if (strcmp(mbuff.mtext.message, "Отключение") == 0)
            {
                int index = 0;
                while(index != count_of_clients && clients[index] != mbuff.mtext.id) index++;
                if (index == count_of_clients) continue;
                for (int j = index; j<count_of_clients-1; j++)
                {
                    clients[j] = clients[j+1];
                }
                count_of_clients--;
                printf("Отключился %d\n", mbuff.mtext.id);
                if (count_of_clients == 0) no_clients = 1; 
                continue;
            }   
            else if (strcmp(mbuff.mtext.message, "Подключение") == 0)
            {
                printf("Подключился %d\n", mbuff.mtext.id);
                if (count_of_clients == MAX_COUNT_CLIENTS)
                   strcpy(mbuff.mtext.message, "Максимальное количество клиентов\n\0");                
                else 
                {
                    clients[count_of_clients++] = mbuff.mtext.id;
                    strcpy(mbuff.mtext.message, "Success");
                }
                mbuff.mtype = mbuff.mtext.id;
                mbuff.mtext.id = 1;
                msgsnd(msg, &mbuff, sizeof(struct Text), 0);
                continue;
            }
            else if (count_of_clients >= 2)
            { 
                snprintf(message, strlen(mbuff.mtext.message)+20, "От %d: %s", mbuff.mtext.id, mbuff.mtext.message);
                strcpy(mbuff.mtext.message, message);
                for (int i = 0; i<count_of_clients; i++)
                {
                    if (clients[i] == mbuff.mtext.id)
                        continue;
                    mbuff.mtype = clients[i];
                    msgsnd(msg, &mbuff, sizeof(struct Text), 0);
                }
            }
        }
    }
    exit(EXIT_SUCCESS);
}
