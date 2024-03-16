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

typedef struct { //word data struct used by storeWords
    char word[MAX_WORD_LENGTH];
    int line;
    int column;
} words;

char wordArray[MAX_DICT_WORDS][MAX_WORD_LENGTH]; //word array where words from txt files are stored
int numwordArray = 0;

void spellChecker(const char* filename);

words* storeWords(const char* filename) { //stores every word from a given filename into a struct with its column and line number
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    words* wordArray = malloc(MAX_DICT_WORDS * sizeof(words));
    if (wordArray == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORD_LENGTH];
    int lineNum = 1;
    while (fgets(line, sizeof(line), file) != NULL && numwordArray < MAX_DICT_WORDS) {
        char* token = strtok(line, " \t\n");
        int columnNum = 1;
        while (token != NULL && numwordArray < MAX_DICT_WORDS) {
            strncpy(wordArray[numwordArray].word, token, MAX_WORD_LENGTH - 1);
            wordArray[numwordArray].word[MAX_WORD_LENGTH - 1] = '\0';
            wordArray[numwordArray].line = lineNum;
            wordArray[numwordArray].column = columnNum;
            numwordArray++;
            token = strtok(NULL, " \t\n");
            columnNum++;
        }
        lineNum++;
    }

    fclose(file);
    return wordArray;
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

    words* wordArray = storeWords(argv[1]);

    printf("Words in the dictionary:\n");
    for (int i = 0; i < numwordArray; i++) {
        printf("Word: %s, Line: %d, Column: %d\n", wordArray[i].word, wordArray[i].line, wordArray[i].column);
    }

    nextFile(argv[1]);

    free(wordArray);

    return 0;
}
