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
} dictWord;

char dictWords[MAX_DICT_WORDS][MAX_WORD_LENGTH];
int numDictWords = 0;

void spellChecker(const char* filename);

dictWord* storeDictWords(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    dictWord* dictWords = malloc(MAX_DICT_WORDS * sizeof(dictWord));
    if (dictWords == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORD_LENGTH];
    int lineNum = 1;
    while (fgets(line, sizeof(line), file) != NULL && numDictWords < MAX_DICT_WORDS) {
        char* token = strtok(line, " \t\n");
        int columnNum = 1;
        while (token != NULL && numDictWords < MAX_DICT_WORDS) {
            strncpy(dictWords[numDictWords].word, token, MAX_WORD_LENGTH - 1);
            dictWords[numDictWords].word[MAX_WORD_LENGTH - 1] = '\0';
            dictWords[numDictWords].line = lineNum;
            dictWords[numDictWords].column = columnNum;
            numDictWords++;
            token = strtok(NULL, " \t\n");
            columnNum++;
        }
        lineNum++;
    }

    fclose(file);
    return dictWords;
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

    dictWord* dictWords = storeDictWords(argv[1]);

    printf("Words in the dictionary:\n");
    for (int i = 0; i < numDictWords; i++) {
        printf("Word: %s, Line: %d, Column: %d\n", dictWords[i].word, dictWords[i].line, dictWords[i].column);
    }

    nextFile(argv[1]);

    free(dictWords);

    return 0;
}
