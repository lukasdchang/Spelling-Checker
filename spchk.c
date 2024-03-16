#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_WORD_LEN 100
#define MAX_LINE_LEN 1000

// Data structure for dictionary (binary search)
char dictionary[MAX_WORD_LEN][MAX_WORD_LEN];
int dict_size = 0;

// Function prototypes
void load_dictionary(const char *dict_file);
int is_word_in_dict(const char *word);
void check_file(const char *file_path);
void process_dir(const char *dir_path);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <dictionary> <file1> [<file2> ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Load dictionary
    load_dictionary(argv[1]);

    // Process each file/directory
    for (int i = 2; i < argc; i++) {
        struct stat path_stat;
        if (stat(argv[i], &path_stat) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISREG(path_stat.st_mode)) {
            // Regular file
            check_file(argv[i]);
        } else if (S_ISDIR(path_stat.st_mode)) {
            // Directory
            process_dir(argv[i]);
        }
    }

    if (dict_size > 0) {
        // Dictionary was successfully loaded
        printf("Dictionary loaded successfully.\n");
    }

    exit(EXIT_SUCCESS);
}

void load_dictionary(const char *dict_file) {
    int fd = open(dict_file, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char word[MAX_WORD_LEN];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, word, MAX_WORD_LEN - 1)) > 0) {
        word[bytes_read] = '\0'; // Null terminate the word
        strcpy(dictionary[dict_size], word);
        dict_size++;
    }

    close(fd);
}

int is_word_in_dict(const char *word) {
    // Binary search
    int low = 0, high = dict_size - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strcmp(dictionary[mid], word);
        if (cmp == 0) {
            return 1; // Word found in dictionary
        } else if (cmp < 0) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return 0; // Word not found
}

void check_file(const char *file_path) {
    int fd = open(file_path, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return;
    }

    char line[MAX_LINE_LEN];
    int line_num = 0;
    while (read(fd, line, MAX_LINE_LEN) > 0) {
        line_num++;
        char *token = strtok(line, " \t\n");
        int col_num = 1;
        while (token != NULL) {
            if (!is_word_in_dict(token)) {
                printf("%s (%d,%d): %s\n", file_path, line_num, col_num, token);
            }
            token = strtok(NULL, " \t\n");
            col_num += strlen(token) + 1; // Move to next column
        }
    }

    close(fd);
}

void process_dir(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_name[0] == '.') {
            continue; // Ignore hidden files/directories
        }

        char path[MAX_LINE_LEN];
        snprintf(path, sizeof(path), "%s/%s", dir_path, entry->d_name);
        struct stat path_stat;
        if (stat(path, &path_stat) == -1) {
            perror("stat");
            continue;
        }

        if (S_ISREG(path_stat.st_mode)) {
            // Regular file
            if (strstr(entry->d_name, ".txt") != NULL) {
                check_file(path);
            }
        } else if (S_ISDIR(path_stat.st_mode)) {
            // Sub-directory
            process_dir(path);
        }
    }

    closedir(dir);
}
