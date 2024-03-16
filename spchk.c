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

typedef struct { // struct to store information about each word. Used by storeWords
    char word[MAX_WORD_LENGTH];
    char file_directory[PATH_MAX]; 
    int line;
    int column;
} words;

words wordArray[MAX_DICT_WORDS]; // array of every word the program finds
int numWordArray = 0;

void spellChecker(const char* filename);

void storeWords(const char* filename) { //stores every single word from the given filename into an array
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
            strncpy(wordArray[numWordArray].file_directory, filename, PATH_MAX); 
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

void nextFile(const char* dirname) { //recursively searches through the given directory name
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

            storeWords(path); //stores all the words along with its info within the current txt file into an array
        }

        // Recursive call for directories
        if (entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", dirname, entity->d_name);

            nextFile(path); //finds the next txt file within the directory
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
    //spellChecker(wordArray); 
    // ^ here we want to be able to call a method called spellChecker which will iterate through the wordArray and binary search through a given dictionary
    //      to see if each word is in the dictionary. If its not, it will print it out along with its information.



    // Print all words stored from the text files formatted like this -> Word: , File Directory:  (Line, Column)
    // This is only here to test if the nextFile and storeWords works
    for (int i = 0; i < numWordArray; i++) {
        printf("Word: %s, File Directory: %s (%d,%d)\n", wordArray[i].word, wordArray[i].file_directory, wordArray[i].line, wordArray[i].column);
    }

    return EXIT_SUCCESS;
}
