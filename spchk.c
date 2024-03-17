#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <ctype.h>

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

char dictionary[MAX_DICT_WORDS][MAX_WORD_LENGTH]; // Array to store dictionary words
int numDictionaryWords = 0;

// Function to read the dictionary file and store words in an array
void readDictionary(const char* filename) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("Failed to open dictionary file");
        exit(EXIT_FAILURE);
    }

    char word[MAX_WORD_LENGTH];
    ssize_t bytes_read;
    while ((bytes_read = read(file, word, sizeof(word))) > 0 && numDictionaryWords < MAX_DICT_WORDS) {
        // Remove newline character from the end of the word
        word[strcspn(word, "\n")] = '\0';
        strcpy(dictionary[numDictionaryWords], word);
        numDictionaryWords++;
    }

    close(file);
}

// Function to perform binary search to check if a word exists in the dictionary
int binarySearch(const char* word) {
    // Implement binary search logic here
}

// Function to check spelling of words in wordArray against the dictionary
void spellChecker() {
    for (int i = 0; i < numWordArray; i++) {
        int index = binarySearch(wordArray[i].word);
        if (index == -1) {
            printf("%s (%s:%d,%d)\n", wordArray[i].word, wordArray[i].file_directory, wordArray[i].line, wordArray[i].column);
        }
    }
}

// Function to store words from a file into wordArray
void storeWords(const char* filename) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORD_LENGTH];
    int lineNum = 1;
    ssize_t bytes_read;
    while ((bytes_read = read(file, line, sizeof(line))) > 0 && numWordArray < MAX_DICT_WORDS) {
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

    close(file);
}

// Function to recursively search through a directory and process text files
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
    if (argc < 3) {
        printf("Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    readDictionary(argv[1]); // Read the dictionary file specified in the first argument
    nextFile(argv[2]); // Process text files specified in the second argument

    // Print all words stored from the text files with line, column, and file directory information
    for(int i = 0; i < numWordArray; i++) {
        printf("Word: %s, File Directory: %s (%d, %d)\n", wordArray[i].word, wordArray[i].file_directory, wordArray[i].line, wordArray[i].column);
    }

    spellChecker(); // Check spelling against the dictionary

    return EXIT_SUCCESS;
}
