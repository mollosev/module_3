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

void clearBuff()
{
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(int argc, char * argv[])
{
    key_t key = ftok("server.txt", 95);
    int msg = msgget(key, 0);
    if (msg == -1) perror(NULL);
    char buff[BUFF_SIZE];
    struct msgbuf mbuff;
    int entered_value;

    pid_t pid = getpid();
    mbuff.mtype = 1;
    strcpy(mbuff.mtext.message, "Подключение");
    mbuff.mtext.id = pid;
    if (msgsnd(msg, &mbuff, sizeof(struct Text), IPC_NOWAIT) <= 0) perror;
    msgrcv(msg, &mbuff, sizeof(struct Text), pid, 0);
    if (strcmp(mbuff.mtext.message, "Success") != 0) { printf("%s\n", mbuff.mtext.message); exit(EXIT_FAILURE); }
    while(1)
    {
        entered_value = 0;
        printf("Прочитать новые сообщения - 1\n");
        printf("Написать сообщение - 2\n");
        printf("Отключиться - 3\n");
        printf("Выбор: ");
        fgets(buff, 2, stdin);
        sscanf(buff, "%d", &entered_value);
        clearBuff();
        switch (entered_value)
        {
        case 1:
        {
            system("clear");
            while(msgrcv(msg, &mbuff, sizeof(struct Text), pid, IPC_NOWAIT) > 0)
            {
                printf("%s", mbuff.mtext.message);
            }
            printf("Больше сообщений нет\n\n");
            printf("Нажмите для возврата в меню...\n");
            getchar();
            break;
        }
        case 2:
            printf("Введите сообщение: ");
            fgets(buff, BUFF_SIZE, stdin);
            mbuff.mtype = 1;
            strcpy(mbuff.mtext.message, buff);
            mbuff.mtext.id = pid;
            if (msgsnd(msg, &mbuff,  sizeof(struct Text), 0) == -1) perror(NULL);
            printf("\n");
            break;
        case 3:
            mbuff.mtype = 1;
            mbuff.mtext.id = pid;
            strcpy(mbuff.mtext.message, "Отключение"); 
            if (msgsnd(msg, &mbuff, sizeof(struct Text), 0)) perror(NULL);
            exit(EXIT_SUCCESS);
        default:
            printf("Неправильный выбор\n\n");
            break;
        }
    }
    exit(EXIT_SUCCESS);
}
