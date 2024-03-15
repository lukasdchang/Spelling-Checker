#include <stdio.h>
#include <string.h>
#include <dirent.h>

void listFiles(const char* dirname){ //function that lists all the files recursively given a directory name
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
                listFiles(path);
            }
        }
        entity = readdir(dir);
    }
    closedir(dir);
}

int main(int argc, char* argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }
    listFiles(argv[1]);
    return 0;
}