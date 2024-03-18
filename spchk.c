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

typedef struct { // struct to store information about each word. Used by readTxt
    char word[MAX_WORD_LENGTH];
    char file_directory[PATH_MAX]; 
    int line;
    int column;
} words;

words wordArray[MAX_DICT_WORDS]; // array of every word the program finds
int numWordArray = 0;

char dictionary[MAX_DICT_WORDS][MAX_WORD_LENGTH]; // Array to store dictionary words
int numDictionaryWords = 0;

void toLowerCase(char *word) {
    for (int i = 0; word[i]; i++) {
        word[i] = tolower(word[i]);
    }
}

int compareWords(const void *a, const void *b) {
    const char *str1 = (const char*)a;
    const char *str2 = (const char*)b;

    //printf("Comparing \"%s\" with \"%s\"\n", str1, str2);

    // Compare directly if exactly the same (covers all caps and exact match)
    if (strcmp(str1, str2) == 0) return 0;
    // Convert to lowercase and compare (covers initial cap and all lowercase)
    char lowerStr1[MAX_WORD_LENGTH], lowerStr2[MAX_WORD_LENGTH];
    strcpy(lowerStr1, str1);
    strcpy(lowerStr2, str2);
    toLowerCase(lowerStr1);
    toLowerCase(lowerStr2);
    
    return strcmp(lowerStr1, lowerStr2);
}

// Function to read the dictionary file and store words in an array
void readDictionary(const char* filename) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("Failed to open dictionary file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORD_LENGTH];
    char buffer[MAX_WORD_LENGTH];
    ssize_t bytes_read;
    ssize_t line_length = 0;
    ssize_t buffer_length = 0;
    while ((bytes_read = read(file, buffer + buffer_length, sizeof(buffer) - buffer_length)) > 0 && numDictionaryWords < MAX_DICT_WORDS) {
        buffer_length += bytes_read;
        char* start = buffer;
        char* end;
        while ((end = memchr(start, '\n', buffer_length - (start - buffer))) != NULL) {
            line_length = end - start;
            strncpy(line, start, line_length);
            line[line_length] = '\0';
            start = end + 1;
            char* token = strtok(line, " \t\n");
            while (token != NULL && numDictionaryWords < MAX_DICT_WORDS) {
                strcpy(dictionary[numDictionaryWords], token);
                numDictionaryWords++;
                token = strtok(NULL, " \t\n");
            }
        }
        buffer_length -= (start - buffer);
        memmove(buffer, start, buffer_length);
    }

    if (bytes_read == -1) {
        perror("Failed to read from dictionary file");
        exit(EXIT_FAILURE);
    }

    close(file);
    qsort(dictionary, numDictionaryWords, MAX_WORD_LENGTH, compareWords); 
}

void readTxt(const char* filename) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_WORD_LENGTH];
    ssize_t bytes_read;
    int lineNum = 1;
    int columnNum = 1;
    int wordStartIndex = 0; // Index to start recording a new word
    while ((bytes_read = read(file, buffer, sizeof(buffer))) > 0 && numWordArray < MAX_DICT_WORDS) {
        for (ssize_t i = 0; i < bytes_read && numWordArray < MAX_DICT_WORDS; i++) {
            char currentChar = buffer[i];
            // Check for word boundaries
            if ((!isalnum(currentChar) && currentChar != '\'' && currentChar != '-') || isdigit(currentChar)) {
                // Non-alphanumeric character or digit indicates end of word
                // New word found, start recording
                if (i > wordStartIndex) { // Avoid recording empty words
                    // Copy the word to the wordArray
                    int length = i - wordStartIndex;
                    if (length >= MAX_WORD_LENGTH) {
                        length = MAX_WORD_LENGTH - 1;
                    }
                    strncpy(wordArray[numWordArray].word, buffer + wordStartIndex, length);
                    wordArray[numWordArray].word[length] = '\0';
                    strncpy(wordArray[numWordArray].file_directory, filename, PATH_MAX);
                    wordArray[numWordArray].line = lineNum;
                    wordArray[numWordArray].column = columnNum - (i - wordStartIndex); // Adjust column number
                    numWordArray++;
                }
                // Reset word start index
                wordStartIndex = i + 1;
            }
            // Keep track of line and column numbers
            if (currentChar == '\n') {
                lineNum++;
                columnNum = 1;
            } else {
                columnNum++;
            }
        }
    }

    if (bytes_read == -1) {
        perror("Failed to read from file");
        exit(EXIT_FAILURE);
    }

    close(file);
}

void printDictionary() { // testing purposes
    printf("Words in the dictionary:\n");
    for (int i = 0; i < numDictionaryWords; i++) {
        printf("%s\n", dictionary[i]);
    }
}

void printWords() {
    for(int i = 0; i < numWordArray; i++) {
        printf("Word: %s, File Directory: %s (%d, %d)\n", wordArray[i].word, wordArray[i].file_directory, wordArray[i].line, wordArray[i].column);
    }
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

            readTxt(path); //stores all the words along with its info within the current txt file into an array
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



void checkSpelling() {
    for (int i = 0; i < numWordArray; i++) {
        //printf("%zu \n", strlen(wordArray[i].word));
        if (!bsearch(wordArray[i].word, dictionary, strlen(wordArray[i].word), MAX_WORD_LENGTH, compareWords)) {
            printf("Misspelled Word: %s, File Directory: %s (%d, %d)\n", wordArray[i].word, wordArray[i].file_directory, wordArray[i].line, wordArray[i].column);
        }
    }
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    readDictionary(argv[1]); // Read the dictionary file specified in the first argument
    nextFile(argv[2]); // Process text files specified in the second argument

    //printDictionary();
    //printWords();

    checkSpelling();

    return EXIT_SUCCESS;
}
