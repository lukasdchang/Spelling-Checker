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

typedef struct {
    char word[MAX_WORD_LENGTH];
    int line;
    int column;
} words;

char wordss[MAX_DICT_WORDS][MAX_WORD_LENGTH];
int numwordss = 0;

void spellChecker(const char* filename);

words* storeWords(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    words* wordss = malloc(MAX_DICT_WORDS * sizeof(words));
    if (wordss == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORD_LENGTH];
    int lineNum = 1;
    while (fgets(line, sizeof(line), file) != NULL && numwordss < MAX_DICT_WORDS) {
        char* token = strtok(line, " \t\n");
        int columnNum = 1;
        while (token != NULL && numwordss < MAX_DICT_WORDS) {
            strncpy(wordss[numwordss].word, token, MAX_WORD_LENGTH - 1);
            wordss[numwordss].word[MAX_WORD_LENGTH - 1] = '\0';
            wordss[numwordss].line = lineNum;
            wordss[numwordss].column = columnNum;
            numwordss++;
            token = strtok(NULL, " \t\n");
            columnNum++;
        }
        lineNum++;
    }

    fclose(file);
    return wordss;
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
            //spellChecker(path);
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

    words* wordss = storeWords(argv[1]);

    printf("Words in the dictionary:\n");
    for (int i = 0; i < numwordss; i++) {
        printf("Word: %s, Line: %d, Column: %d\n", wordss[i].word, wordss[i].line, wordss[i].column);
    }

    nextFile(argv[1]);

    free(wordss);

    return 0;
}
