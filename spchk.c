#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

//Component 1

void nextFile(const char* dirname){ //function that lists all the files recursively given a directory name
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        perror("opendir");
        return;
    }
    struct dirent* entity;
    entity = readdir(dir);
    while (entity != NULL) {
        if(entity->d_name[0] != '.') {
            printf("%hhd %s/%s\n", entity->d_type, dirname, entity->d_name);
            if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0){
                char path[100] = { 0 };
                strcat(path, dirname);
                strcat(path, "/");
                strcat(path, entity->d_name);
                nextFile(path);
            }
        }
        entity = readdir(dir);
    }
    closedir(dir);
}

//Component 2
typedef struct {
    char* word;
    int line;
    int column;
} Word;



//Component 3



int main(int argc, char* argv[]) {
    if(argc != 2) {
        printf("Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }
    nextFile(argv[1]);
    return 0;
}