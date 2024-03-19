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

typedef struct { // struct to store information about each word, used by readTextFile
    char word[MAX_WORD_LENGTH];
    char file_directory[PATH_MAX]; 
    int line;
    int column;
} words;

words wordArray[MAX_DICT_WORDS]; // array of every word the program finds from text files
int wordArrayLength = 0;

char dictArray[MAX_DICT_WORDS][MAX_WORD_LENGTH]; // array of every word in the dictionary file
int dictArrayLength = 0;

// function that converts two strings to it's lowercase versions. they match, it checks for capitalization by making all variations of acceptable capitalization
// and checking if they match the first string. if they don't match it returns the lexical difference of the 2 strings for bsearch() in spellChecker()
int compareWords(const void *a, const void *b) { 
    const char *str1 = (const char*)a;
    const char *str2 = (const char*)b;
    char str1Lower[MAX_WORD_LENGTH];
    char str2Lower[MAX_WORD_LENGTH];
    char str2Variations[3][MAX_WORD_LENGTH]; // To store variations of acceptable capitalization for str2
    int variationCount = 0;

    // Convert str1 and str2 to lowercase for comparison
    for (int i = 0; str1[i]; i++) {
        str1Lower[i] = tolower(str1[i]);
        str1Lower[i+1] = '\0'; // Ensure null-termination
    }
    for (int i = 0; str2[i]; i++) {
        str2Lower[i] = tolower(str2[i]);
        str2Lower[i+1] = '\0'; // Ensure null-termination
    }

    // If the lowercase versions match, check for capitalization
    if (strcmp(str1Lower, str2Lower) == 0) {
        // Always add the exact match (str2) as a valid variation
        strcpy(str2Variations[variationCount++], str2);

        // If str2 starts with an uppercase letter, add uppercase version as well
        if (isupper(str2[0])) {
            for (int i = 0; str2[i]; i++) {
                str2Variations[variationCount][i] = toupper(str2[i]);
                str2Variations[variationCount][i+1] = '\0'; // Ensure null-termination
            }
            variationCount++;
        } else { // If str2 starts with a lowercase letter, add "Hello" and "HELLO" variations
            // Initial capital variation
            strcpy(str2Variations[variationCount], str2);
            str2Variations[variationCount][0] = toupper(str2Variations[variationCount][0]);
            variationCount++;

            // All caps variation
            for (int i = 0; str2[i]; i++) {
                str2Variations[variationCount][i] = toupper(str2[i]);
                str2Variations[variationCount][i+1] = '\0'; // Ensure null-termination
            }
            variationCount++;
        }

        // Check if str1 matches any of the acceptable variations of str2
        //printf("Str1: %s, Str2: %s\n", str1, str2);
        for (int i = 0; i < variationCount; i++) {
            //printf("%s\n", str2Variations[i]);
            if (strcmp(str1, str2Variations[i]) == 0) {
                return 0; // Match found
            }
        }

        return -1; // No acceptable match found
    } else {
        // If the lowercase versions do not match, return the lexical difference
        return strcmp(str1Lower, str2Lower);
    }
}

// function to read the dictArray file and store words in an array
void readDictionaryFile(const char* filename) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("Failed to open dictArray file");
        exit(EXIT_FAILURE);
    }

    char line[MAX_WORD_LENGTH];
    char buffer[MAX_WORD_LENGTH];
    ssize_t bytes_read;
    ssize_t line_length = 0;
    ssize_t buffer_length = 0;

    //read the file content into the buffer in chunks
    while ((bytes_read = read(file, buffer + buffer_length, sizeof(buffer) - buffer_length)) > 0 && dictArrayLength < MAX_DICT_WORDS) {
        buffer_length += bytes_read;
        char* start = buffer;
        char* end;

        // process each line in the buffer
        while ((end = memchr(start, '\n', buffer_length - (start - buffer))) != NULL) {
            line_length = end - start;
            strncpy(line, start, line_length);
            line[line_length] = '\0';
            start = end + 1;
            char* token = strtok(line, " \t\n"); // break up text file into smaller strings separated by whitespaces using a tokenizer
            while (token != NULL && dictArrayLength < MAX_DICT_WORDS) { 
                strcpy(dictArray[dictArrayLength], token); // copy the tokenized word into dictArray
                dictArrayLength++;
                token = strtok(NULL, " \t\n");
            }
        }
        buffer_length -= (start - buffer); // updates buffer length
        memmove(buffer, start, buffer_length); // move remaining content to buffer to the beginning
    }

    if (bytes_read == -1) {
        perror("Failed to read from dictArray file");
        exit(EXIT_FAILURE);
    }

    close(file);

    // sorts the dictionary array alphabetically
    qsort(dictArray, dictArrayLength, MAX_WORD_LENGTH, compareWords); 
}

void readTextFile(const char* filename) {
    int file = open(filename, O_RDONLY);
    if (file == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    char buffer[MAX_WORD_LENGTH];
    ssize_t bytes_read;
    int lineNum = 1;
    int columnNum = 1;
    int wordStartIndex = 0;

    while ((bytes_read = read(file, buffer, sizeof(buffer))) > 0 && wordArrayLength < MAX_DICT_WORDS) {
        for (ssize_t i = 0; i < bytes_read && wordArrayLength < MAX_DICT_WORDS; i++) {
            char currentChar = buffer[i];

            if ((!isalnum(currentChar) && currentChar != '\'' && currentChar != '-') || isdigit(currentChar)) {
                if (i > wordStartIndex) { 
                    int length = i - wordStartIndex;
                    if (length >= MAX_WORD_LENGTH) {
                        length = MAX_WORD_LENGTH - 1;
                    }

                    // Copy the word into wordArray
                    strncpy(wordArray[wordArrayLength].word, buffer + wordStartIndex, length);
                    wordArray[wordArrayLength].word[length] = '\0'; // Null terminate word

                    // Check for hyphenated words
                    char *hyphenatedWord = strchr(wordArray[wordArrayLength].word, '-');
                    if (hyphenatedWord != NULL) {
                        // Split the hyphenated word and check each component
                        char *component = strtok(wordArray[wordArrayLength].word, "-");
                        while (component != NULL) {
                            if (bsearch(component, dictArray, dictArrayLength, MAX_WORD_LENGTH, compareWords) == NULL) {
                                // If any component is not found, mark the word as misspelled
                                //printf("%s (%d, %d): %s\n", wordArray[wordArrayLength].file_directory, wordArray[wordArrayLength].line, wordArray[wordArrayLength].column, wordArray[wordArrayLength].word);
                                break;
                            }
                            component = strtok(NULL, "-");
                        }
                    }

                    // Store word data
                    strncpy(wordArray[wordArrayLength].file_directory, filename, PATH_MAX); // File directory
                    wordArray[wordArrayLength].line = lineNum; // Line number
                    wordArray[wordArrayLength].column = columnNum - (i - wordStartIndex); // Column number

                    wordArrayLength++;
                }
                wordStartIndex = i + 1; // Next word
            }
            // Update line and column numbers
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



// function to recursively search through a directory and process text files given a directory name
void nextFile(const char* dirname) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent* entity;
    entity = readdir(dir);
    while (entity != NULL) {
        // check if the entity is a regular file and ends with ".txt"
        if (entity->d_type == DT_REG && strstr(entity->d_name, ".txt") != NULL) {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", dirname, entity->d_name);

            readTextFile(path); // stores all the words along with its info within the current txt file into an array
        }

        // recursive call for directories
        if (entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0) {
            char path[PATH_MAX];
            snprintf(path, PATH_MAX, "%s/%s", dirname, entity->d_name);

            nextFile(path); // finds the next txt file within the directory
        }

        entity = readdir(dir);
    }

    closedir(dir);
}

void checkSpelling() { // uses binary search (bsearch()) to check whether words in the wordArray exist in the dictArray
    for (int i = 0; i < wordArrayLength; i++) {
        if (bsearch(wordArray[i].word, dictArray, dictArrayLength, MAX_WORD_LENGTH, compareWords) == NULL) {
            // prints words formatted: file directory (line, column): word 
            //printf("%s (%d, %d): %s\n", wordArray[i].file_directory, wordArray[i].line, wordArray[i].column, wordArray[i].word);
        }
    }
}

// prints out dictArray (testing purposes)
void printDictArray() { 
    printf("Words in the dictArray:\n");
    for (int i = 0; i < dictArrayLength; i++) {
        printf("%s\n", dictArray[i]);
    }
}

// prints out wordArray (testing purposes)
void printWordArray() { 
    for(int i = 0; i < wordArrayLength; i++) {
        printf("Word: %s, File Directory: %s (%d, %d)\n", wordArray[i].word, wordArray[i].file_directory, wordArray[i].line, wordArray[i].column);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Invalid number of arguments.\n");
        return EXIT_FAILURE;
    }

    readDictionaryFile(argv[1]); // Read the dictArray file specified in the first argument
    nextFile(argv[2]); // Process text files specified in the second argument

    //printDictArray();
    printWordArray();

    checkSpelling(); 

    return EXIT_SUCCESS;
}
