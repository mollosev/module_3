#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define LOCAL_PORT 51050

int main(int argc, char **argv) {
    int sockfd; /* Дескриптор сокета */
    int n, len; /* Переменные для различных длин и количества символов */
    char sendline[1000], recvline[1000]; /* Массивы для отсылаемой и принятой строки */
    struct sockaddr_in servaddr, cliaddr; /* Структуры для адресов клиентов */

    if(argc != 3) {
        printf("Usage: a.out <IP address> <PORT>\n"); // Указываем удаленный порт
        exit(1);
    }

    /* Создаем UDP сокет */
    if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("SOCKET"); /* Печатаем сообщение об ошибке */
        exit(1);
    }

    /* Заполняем структуру для адреса клиента */
    bzero(&cliaddr, sizeof(cliaddr));
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_port = htons(LOCAL_PORT); /* Локальный порт */
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    /* Настраиваем адрес сокета */
    if(bind(sockfd, (struct sockaddr *) &cliaddr, sizeof(cliaddr)) < 0) {
        perror(NULL);
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(1);
    }

    /* Заполняем структуру для адреса удаленного клиента */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2])); /* Удаленный порт */
    if(inet_aton(argv[1], &servaddr.sin_addr) == 0) {
        printf("Invalid IP address\n");
        close(sockfd); /* По окончании работы закрываем дескриптор сокета */
        exit(1);
    }
    while(1) {
        /* Вводим строку, которую отошлем другому клиенту */
        printf("String => ");
        fgets(sendline, 1000, stdin);
        /* Отсылаем датаграмму */
        if(sendto(sockfd, sendline, strlen(sendline)+1, 0, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
            perror(NULL);
            close(sockfd);
            exit(1);
        }
        if(strcmp(sendline, "Пока\n") == 0) {
            close(sockfd);
            exit(0);
        }
        /* Ожидаем ответа и читаем его. Максимальная
        допустимая длина датаграммы – 1000 символов,
        адрес отправителя нам не нужен */
        if((n = recvfrom(sockfd, recvline, 1000, 0, (struct sockaddr *) NULL, NULL)) < 0) {
            perror(NULL);
            close(sockfd);
            exit(1);
        }
        /* Печатаем пришедший ответ */
        printf("From: %s\n", recvline);
    } 
    exit(0);
}