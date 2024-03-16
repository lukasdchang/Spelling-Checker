#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

//Component 1

void nextFile(const char* dirname) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent* entity;
    entity = readdir(dir);
    while (entity != NULL) {
        // Check if the entity is a regular file and ends with ".txt"
        if (entity->d_type == DT_REG && strstr(entity->d_name, ".txt") != NULL) {
            printf("%s/%s\n", dirname, entity->d_name);
        }

        // Recursive call for directories
        if (entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            char path[100] = { 0 };
            strcat(path, dirname);
            strcat(path, "/");
            strcat(path, entity->d_name);
            nextFile(path);
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