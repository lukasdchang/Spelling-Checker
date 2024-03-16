#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#define MAX_WORD_LENGTH 100
#define MAX_DICT_WORDS 200000

typedef struct {
    char word[MAX_WORD_LENGTH];
    char file_directory[PATH_MAX]; // Store the file directory
    int line;
    int column;
} words;

words wordArray[MAX_DICT_WORDS];
int numWordArray = 0;

void spellChecker(const char* filename);

void storeWords(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORD_LENGTH];
    int lineNum = 1;
    while (fgets(line, sizeof(line), file) != NULL && numWordArray < MAX_DICT_WORDS) {
        char* token = strtok(line, " \t\n");
        int columnNum = 1;
        while (token != NULL && numWordArray < MAX_DICT_WORDS) {
            strcpy(wordArray[numWordArray].word, token);
            strncpy(wordArray[numWordArray].file_directory, filename, PATH_MAX); // Store the file directory
            wordArray[numWordArray].line = lineNum;
            wordArray[numWordArray].column = columnNum;
            numWordArray++;
            token = strtok(NULL, " \t\n");
            columnNum++;
        }
        lineNum++;
    }

    fclose(file);
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
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", dirname, entity->d_name);
            storeWords(path);
        }

        // Recursive call for directories
        if (entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", dirname, entity->d_name);
            nextFile(path);
        }

        entity = readdir(dir);
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    nextFile(argv[2]);

    // Print all words stored from the text files with line, column, and file directory information
    for (int i = 0; i < numWordArray; i++) {
        printf("Word: %s, File Directory: %s (%d,%d)\n", wordArray[i].word, wordArray[i].file_directory, wordArray[i].line, wordArray[i].column);
    }

    return EXIT_SUCCESS;
}
