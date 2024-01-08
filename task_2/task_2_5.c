#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char * argv[]) {
    int i = 1;
    FILE * file;
    while(1) {
        if((file = fopen("task_2_5.txt", "a+")) == NULL) {
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
