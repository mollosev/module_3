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

void clearBuff();

struct msgbuf {
    long mtype;
    char mtext[1];
};

int main(int argc, char * argv[]) {
    key_t key = ftok("queue.txt", 95);
    int msg = msgget(key, 0);
    if (msg == -1) perror(NULL);
    char buff[BUFF_SIZE];
    struct msgbuf mbuff;
    int entered_value;
    while(1) {
        entered_value = 0;
        printf("Прочитать новые сообщения - 1\n");
        printf("Написать сообщение - 2\n");
        printf("Выбор: ");
        fgets(buff, 2, stdin);
        sscanf(buff, "%d", &entered_value);
        clearBuff();
        switch (entered_value) {
            case 1: {
                system("clear");
                while(msgrcv(msg, &mbuff, BUFF_SIZE, 2, IPC_NOWAIT | MSG_EXCEPT) > 0) {
                    printf("Полученное сообщение: %s", mbuff.mtext);
                    if(strcmp(mbuff.mtext, "Пока!\n") == 0) {
                        mbuff.mtype = 256;
                        if(msgsnd(msg, &mbuff, BUFF_SIZE, 0) == -1) perror(NULL);   
                        exit(EXIT_SUCCESS);                
                    }
                }
                printf("Больше сообщений нет\n\n");
                printf("Нажмите для возврата в меню...\n");
                getchar();
                break;
            }
            case 2:
                printf("Введите сообщение: ");
                fgets(buff, BUFF_SIZE, stdin);
                mbuff.mtype = 2;
                strcpy(mbuff.mtext, buff);
                if(msgsnd(msg, &mbuff, BUFF_SIZE, 0) == -1) perror(NULL);
                printf("\n");
                break;
            default:
                printf("Неправильный выбор\n\n");
                break;
        }
    }
    exit(EXIT_SUCCESS);
}

void clearBuff() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}