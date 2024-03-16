#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_WORD_LENGTH 100
#define MAX_DICT_WORDS 200000

char dictWords[MAX_DICT_WORDS][MAX_WORD_LENGTH];
int numDictWords = 0;

typedef struct{
    int word;
    int line;
    int column;
} dictWord;

void spellChecker(const char* filename);

void readDict(const char* filename) {
    int dictionary = open(filename, O_RDONLY);
    if (dictionary == -1) {
        perror("Failed to open dictionary file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORD_LENGTH];
    ssize_t bytesRead;
    while ((bytesRead = read(dictionary, line, sizeof(line))) > 0 && numDictWords < MAX_DICT_WORDS) {
        // Ensure null-termination of the string
        line[bytesRead] = '\0';

        // Split the input into lines and store each line as a word
        char* token = strtok(line, "\n");
        while (token != NULL && numDictWords < MAX_DICT_WORDS) {
            strncpy(dictWords[numDictWords++], token, MAX_WORD_LENGTH - 1);
            token = strtok(NULL, "\n");
        }
    }

    if (bytesRead == -1) {
        perror("Error while reading dictionary file");
        exit(EXIT_FAILURE);
    }

    close(dictionary);
}

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
            //printf("%s/%s\n", dirname, entity->d_name);
            char path[100] = { 0 };
            strcat(path, dirname);
            strcat(path, "/");
            strcat(path, entity->d_name);

            // Call the spell checker method for the current .txt file
            spellChecker(path);
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

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    readDict(argv[1]);

    printf("Words in the dictionary:\n");
    for (int i = 0; i < MAX_DICT_WORDS && dictWords[i][0] != '\0'; i++) {
        printf("%s\n", dictWords[i]);
    }

    nextFile(argv[1]);
    return 0;
}