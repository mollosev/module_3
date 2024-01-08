#include <signal.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

void printSignal(int signal);

int sigintRec = 0;
int sigquitRec = 0;

int main(int argc, char * argv[]) {
    int i = 1;
    FILE * file;
    signal(SIGINT, printSignal);
    signal(SIGQUIT, printSignal);
    while(1) {
        while(sigintRec) { 
            printf("Получен SIGINT\n");
            sigintRec--; 
        }
        while(sigquitRec) {
            printf("Получен SIGQUIT\n"); 
            sigquitRec--; 
        }
        if((file = fopen("task_2_7.txt", "a+")) == NULL) {
            perror(NULL);
            exit(EXIT_FAILURE);
        }
        fprintf(file, "%d ", i);
        if (i % 10 == 0) fprintf(file, "\n");
        i++;
        fclose(file);
        sleep(1);
    }
    exit(EXIT_SUCCESS);
}

void printSignal(int signal) {
    if(signal == SIGQUIT)
        sigquitRec++;
    if(signal == SIGINT)
        sigintRec++;
}