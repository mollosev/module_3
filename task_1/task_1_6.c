#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define START_BOLD "\e[1;33m" 
#define END_BOLD "\e[m"
#define SUCCESS 0
#define FAILURE 1

char* nextDir(char *current_dit);
int printDir(char *dir_path);

int main(int argc, char *argv[]) {
    char *dir_path;
    if (argc > 1) {
        dir_path = malloc(sizeof(char)*(strlen(argv[1])+1));
        strcpy(dir_path, argv[1]);
    } 
    else {
        dir_path = malloc(sizeof(char)*2);
        strcpy(dir_path, "/");
    }
    char *prev_dir;
    if (printDir(dir_path) == FAILURE)
        exit(EXIT_FAILURE);
    else {
        prev_dir = dir_path;
        dir_path = nextDir(dir_path);
    }
    while(1) {
        if (printDir(dir_path) == FAILURE) {
            dir_path = prev_dir;
            prev_dir = NULL;
            continue;
        }
        free(prev_dir);
        prev_dir = dir_path;
        dir_path = nextDir(dir_path);
    }
    exit(EXIT_SUCCESS);
}

char* nextDir(char *current_dit) {
    char next_dir[15], buff[20];
    printf("Введите имя каталога: ");
    fgets(buff, 15, stdin);
    sscanf(buff, "%s", next_dir);
    unsigned int size = strlen(current_dit) + strlen(next_dir) + 2;
    char *path = malloc(sizeof(char)*size);
    memset(path, '\0', size);
    if (strcmp(next_dir, ".") == 0) return current_dit; // Если ввели точку, остаемся в том же каталоге
    if (strcmp(next_dir, "..") == 0) {
        if (strcmp(current_dit, "/") != 0) { // Ввели .. и каталог не корневой, то переходим выше
            int index = strlen(current_dit);
            while (current_dit[index] != '/'){
                index--;
            }
            if (index == 0) strcpy(path, "/");
            else strncpy(path, current_dit, index);
            path[index+1] = '\0';
        } 
        else return current_dit; // Каталог корневой - остаемся
    } 
    else if (strcmp(current_dit, "/") != 0)
        strcat(strcat(strcat(path, current_dit), "/"), next_dir);
    else
        strcat(strcat(path, current_dit), next_dir); // К корневому не добавляем "/"
    return path;
}

int printDir(char *dir_path) {
    struct stat file_stat;
    struct dirent *file;
    DIR *dir;
    if ((dir = opendir(dir_path)) == NULL) {
        perror("Ошибка открытия каталога");
        printf("Нажмите для продолжения");
        getchar();
        return FAILURE;
    }
    system("clear");
    printf("Текущий каталог: %s\n", dir_path);
    while ((file = readdir(dir)) != NULL) {
        if (file->d_name[0] == '.') continue;
        unsigned int size = strlen(dir_path) + strlen(file->d_name) + 2; // Дополнительные два символа в объединенный путь: / и \0
        char path[size];
        memset(path, '\0', size);
        strcat(strcat(strcat(path, dir_path), "/"), file->d_name);
        if (stat(path ,&file_stat) != 0) 
            perror(NULL);
        if (S_ISREG(file_stat.st_mode)) printf(START_BOLD "%ho %s\n" END_BOLD, file_stat.st_mode & 511, file->d_name);
        else printf("%ho %s\n", file_stat.st_mode & 511, file->d_name);
    }
    closedir(dir);
    return SUCCESS;
}