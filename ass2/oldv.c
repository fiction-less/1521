////////////////////////////////////////////////////////////////////////
// COMP1521 22T2 --- Assignment 2: `bytelocker', a simple file encryptor
// <https://www.cse.unsw.edu.au/~cs1521/22T2/assignments/ass2/index.html>
//
// Written by Kelly Xu (z5285375) on 3-AUG-2022.
//
// 2022-07-22   v1.2    Team COMP1521 <cs1521 at cse.unsw.edu.au>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "bytelocker.h"


// ADD ANY EXTRA #defines HERE
#define MAX_DIR_NAME 50


char *generate_random_string(int seed);
void sort_by_count(struct text_find *files, size_t n);
void sort_by_name(char *filenames[], size_t num_files);

// ADD YOUR FUNCTION PROTOTYPES HERE
void print_perms(mode_t m);

// Some provided strings which you may find useful. Do not modify.
const char *const MSG_ERROR_FILE_STAT  = "Could not stat file\n";
const char *const MSG_ERROR_FILE_OPEN  = "Could not open file\n";
const char *const MSG_ERROR_CHANGE_DIR = "Could not change directory\n";
const char *const MSG_ERROR_DIRECTORY  =
    "%s cannot be encrypted: bytelocker does not support encrypting directories.\n";
const char *const MSG_ERROR_READ       =
    "%s cannot be encrypted: group does not have permission to read this file.\n";
const char *const MSG_ERROR_WRITE      =
    "%s cannot be encrypted: group does not have permission to write here.\n";
const char *const MSG_ERROR_SEARCH     = "Please enter a search string.\n";
const char *const MSG_ERROR_RESERVED   =
    "'.' and '..' are reserved filenames, please search for something else.\n";


//////////////////////////////////////////////
//                                          //
//              SUBSET 0                    //
//                                          //
//////////////////////////////////////////////

//
//  Read the file permissions of the current directory and print them to stdout.
//


void show_perms(char filename[MAX_PATH_LEN]) {
    struct stat s;

    // error checking
    if (stat(filename, &s) != 0) {
        fprintf(stdout, "%s", MSG_ERROR_FILE_STAT);
        perror(filename);
        return;
    }


    mode_t m = s.st_mode;

    // check if directory or file
    printf("%s: ", filename);
    print_perms(m);
    printf("\n");

}

//
//  Prints current working directory to stdout.
//
void print_current_directory(void) {
    char cwd[MAX_PATH_LEN];

    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("cwd");
        return;
    }

    if (strcmp(cwd, "~") == 0) {
        // get the home dir
        char *home =  getenv("HOME");
        printf("Current directory is: %s\n", home);
        return;
    }

    printf("Current directory is: %s\n", cwd);


}

//
//  Changes directory to the given path.
//
void change_directory(char dest_directory[MAX_PATH_LEN]) {

    // struct stat s;

    // // check if it is a directory
    // mode_t m = s.st_mode;
    // if (!S_ISDIR(m)) printf("Could not change directory\n");

    // change directory
    if (chdir(dest_directory) != 0) {
        fprintf(stderr, MSG_ERROR_CHANGE_DIR);
    }

    if (strcmp(dest_directory, "~") == 0) {
        // get the home dir
        char *home =  getenv("HOME");
        chdir(home);
        printf("Moving to %s\n", home);
        return;
    }


    // if ..
    // if .

    printf("Moving to %s\n", dest_directory);
}

//
//  Lists the contents of the current directory to stdout.
//
void list_current_directory(void) {

    char cwd[MAX_PATH_LEN];
    DIR *dirp = opendir(getcwd(cwd, sizeof(cwd)));
    printf("%s\n", cwd);



    if (dirp == NULL) {
        perror("");  // prints why the open failed
        return;
    }

    struct dirent *dir;

    // store all dir names in an array
    int i = 0;
    char *name[MAX_NUM_FINDS];

    // store dirs and file names into a list
    while ((dir = readdir(dirp)) != NULL) {
        name[i] = (char*) malloc (strlen(dir->d_name)+1);
        strncpy (name[i],dir->d_name, strlen(dir->d_name) );
        i++;
    }

    int len = i;
    sort_by_name(name, len);

    //printf("%d", len);
    for (i = 0; i < len; i++) {
        struct stat s;
        //printf("TYUE  \n");

        printf("eorror code %d\n",stat(name[i], &s));
        if (stat(name[i], &s) != 0) {
            fprintf(stdout,"%s", MSG_ERROR_FILE_STAT);
            perror(" ");
            return;
        }
        mode_t m = s.st_mode;
        print_perms(m);
        printf("\t%s\n", name[i]);

    }
    printf("END\n");
    closedir(dirp);


}


//////////////////////////////////////////////
//                                          //
//              SUBSET 1                    //
//                                          //
//////////////////////////////////////////////
bool test_can_encrypt(char filename[MAX_PATH_LEN]) {

    bool res = false;
    struct stat s;
    // error checking:

    // file must exist
    if (stat(filename, &s) != 0) {
        fprintf(stdout, "%s", MSG_ERROR_FILE_STAT);
        perror(filename);
        return res;
    }

    mode_t m = s.st_mode;

    // must be a file
    if (!S_ISREG(m)) {
        fprintf(stdout, MSG_ERROR_DIRECTORY, filename);
        perror(filename);
        return res;
    }

    // group must be able to read file
    if (!(m & S_IRGRP)) {
        fprintf(stdout, MSG_ERROR_READ, filename);
        perror(filename);
        return res;
    }


    // group must have perm to write to file
    if (!(m & S_IWGRP)) {
        fprintf(stdout, MSG_ERROR_WRITE, filename);
        perror(filename);
        return res;
    }

    return true;
}

void simple_xor_encryption(char filename[MAX_PATH_LEN]) {

    if (!test_can_encrypt(filename)) return;

    //create filename.xor name
    char XORAppend[MAX_PATH_LEN] = ".xor";
    char XORFile[MAX_PATH_LEN];
    strcpy(XORFile, filename);
    strcat(XORFile, XORAppend);

    // read each byte from the file
    FILE *inputStream = fopen(filename, "rb"); // rb = open the file for reading
    FILE *outputStream = fopen(XORFile, "w");  // opens/creates filename.xor for writing

    if (inputStream == NULL) {
        perror(filename);
        return;
    }

    if (outputStream == NULL) {
        perror(XORFile);
        return;
    }

    int c;
    int temp = 0;
    // read byes until EOF and XOR it with 0FFx
    while ((c = fgetc(inputStream)) != EOF) {
        temp = c ^ 0xFF;
        fputc(temp, outputStream);
    }

    fclose(inputStream);
    fclose(outputStream);

}


void simple_xor_decryption(char filename[MAX_PATH_LEN]) {
    if (!test_can_encrypt(filename)) return;


    //create filename.xor name
    char decAppend[MAX_PATH_LEN] = ".dec";
    char decFile[MAX_PATH_LEN];
    strcpy(decFile, filename);
    strcat(decFile, decAppend);

    // read each byte from the file
    FILE *inputStream = fopen(filename, "rb");
    FILE *outputStream = fopen(decFile, "w");

    // error checking
    if (inputStream == NULL) {
        perror(filename);
        return;
    }

    if (outputStream == NULL) {
        perror(decFile);
        return;
    }


    int c;
    int temp = 0;
    // read byes until EOF and XOR it with 0FFx to undo encryption
    while ((c = fgetc(inputStream)) != EOF) {
        temp = c ^ 0xFF;
        fputc(temp, outputStream);
    }

    fclose(inputStream);
    fclose(outputStream);
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 2                    //
//                                          //
//////////////////////////////////////////////
void recurseDirs(char* src, char desFile[MAX_SEARCH_LENGTH]) {

    //!!!!!WORKS BUT NOT YET IN ALPAHBETICAL ORDER

    DIR *dirp = opendir (src);

    if (dirp == NULL) {
        perror("");  // prints why the open failed
        return;
    }
    struct dirent *dir;
    while ((dir = readdir(dirp)) != NULL) {
        struct stat s;

        // error checking
        if (stat(dir->d_name, &s) != 0) {
            fprintf(stderr, "Could not stat file\n");
            perror(dir->d_name);
            return;
        }
        mode_t m = s.st_mode;

        char *ret = strstr(dir->d_name, desFile);
        if (ret) {
            print_perms(m);
            printf("\t./%s\n", dir->d_name);
        }

        // if its a directory, search it
        if (S_ISDIR(m)) {
            // ignore . & .. since reading dir picks up these
            char path[MAX_PATH_LEN];
            if (strcmp(dir->d_name, ".") == 0 ||
                strcmp(dir->d_name, "..") == 0) continue;

            // create the new dir path
            snprintf(path, sizeof(path), "%s/%s", src, dir->d_name);
            recurseDirs(path, desFile);
        }

    }

    closedir(dirp);
}

// goes through all files and dirs and reads the files
// void readAllFiles();
void search_by_filename(char filename[MAX_SEARCH_LENGTH]) {

    // error checking
    if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {

    }

    char *home =  getenv("HOME");
    recurseDirs(home, filename);
    // start recursion from home

}


void search_by_text(char text[MAX_SEARCH_LENGTH]) {
    printf("TODO: COMPLETE ME\n");
    exit(1);
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 3                    //
//                                          //
//////////////////////////////////////////////
void electronic_codebook_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;

    printf("TODO: COMPLETE ME\n");
    exit(1);
}

void electronic_codebook_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;

    printf("TODO: COMPLETE ME\n");
    exit(1);
}

char *shift_encrypt(char *plaintext, char *password) {
    printf("TODO: COMPLETE ME\n");
    exit(1);
    return NULL;
}

char *shift_decrypt(char *ciphertext, char *password) {
    printf("TODO: COMPLETE ME\n");
    exit(1);
    return NULL;
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 4                    //
//                                          //
//////////////////////////////////////////////
void cyclic_block_shift_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;

    printf("TODO: COMPLETE ME\n");
    exit(1);
}

void cyclic_block_shift_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;

    printf("TODO: COMPLETE ME\n");
    exit(1);
}


// PROVIDED FUNCTIONS, DO NOT MODIFY

// Generates a random string of length RAND_STR_LEN.
// Requires a seed for the random number generator.
// The same seed will always generate the same string.
// The string contains only lowercase + uppercase letters,
// and digits 0 through 9.
// The string is returned in heap-allocated memory,
// and must be freed by the caller.
char *generate_random_string(int seed) {
    if (seed != 0) {
        srand(seed);
    }

    char *alpha_num_str =
            "abcdefghijklmnopqrstuvwxyz"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "0123456789";

    char *random_str = malloc(RAND_STR_LEN);

    for (int i = 0; i < RAND_STR_LEN; i++) {
        random_str[i] = alpha_num_str[rand() % (strlen(alpha_num_str) - 1)];
    }

    return random_str;
}

// Sorts the given array (in-place) of files with
// associated counts into descending order of counts.
// You must provide the size of the array as argument `n`.
void sort_by_count(struct text_find *files, size_t n) {
    if (n == 0 || n == 1) return;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (files[j].count < files[j + 1].count) {
                struct text_find temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            } else if (files[j].count == files[j + 1].count && strcmp(files[j].path, files[j + 1].path) > 0) {
                struct text_find temp = files[j];
                files[j] = files[j + 1];
                files[j + 1] = temp;
            }
        }
    }
}

// Sorts the given array (in-place) of strings alphabetically.
// You must provide the size of the array as argument `n`.
void sort_by_name(char *filenames[], size_t num_filenames) {
    if (num_filenames == 0 || num_filenames == 1) return;
    for (int i = 0; i < num_filenames - 1; i++) {
        for (int j = 0; j < num_filenames - i - 1; j++) {
            if (strcmp(filenames[j], filenames[j + 1]) > 0) {
                char *temp = filenames[j];
                filenames[j] = filenames[j + 1];
                filenames[j + 1] = temp;
            }
        }
    }
}

// ADD YOUR FUNCTION DEFINITIONS HERE

void print_perms(mode_t m) {

    S_ISREG(m) ? printf("-") : printf("d");

    // user permissions
    m & S_IRUSR ? printf("r") : printf("-");
    m & S_IWUSR ? printf("w") : printf("-");
    m & S_IXUSR ? printf("x") : printf("-");
    // group permissions
    m & S_IRGRP ? printf("r") : printf("-");
    m & S_IWGRP ? printf("w") : printf("-");
    m & S_IXGRP ? printf("x") : printf("-");

    // other permissions
    m & S_IROTH ? printf("r") : printf("-");
    m & S_IWOTH ? printf("w") : printf("-");
    m & S_IXOTH ? printf("x") : printf("-");
}