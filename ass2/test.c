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


char *generate_random_string(int seed);
void sort_by_count(struct text_find *files, size_t n);
void sort_by_name(char *filenames[], size_t num_files);

// ADD YOUR FUNCTION PROTOTYPES HERE


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

void print_perms(mode_t m) {

    S_ISREG(m) ? printf("-") : printf("d");

    // user permissions
    m & S_IRUSR ? printf("r") : printf("-");
    m & S_IWUSR ? printf("w") : printf("-");
    m & S_IXUSR ? printf("x") : printf("-");
    // file permissions
    m & S_IRGRP ? printf("r") : printf("-");
    m & S_IWGRP ? printf("w") : printf("-");
    m & S_IXGRP ? printf("x") : printf("-");

    // four permissions
    m & S_IROTH ? printf("r") : printf("-");
    m & S_IWOTH ? printf("w") : printf("-");
    m & S_IXOTH ? printf("x") : printf("-");
}
void show_perms(char filename[MAX_PATH_LEN]) {
    struct stat s;

    // error checking
    if (stat(filename, &s) != 0) {
        printf("Could not stat file\n");
        perror(filename);
        exit(1);
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
        perror(cwd);
        exit(1);
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
        fprintf(stderr, "Could not change directory\n");
    }

    printf("Moving to %s\n", dest_directory);
}

//
//  Lists the contents of the current directory to stdout.
//
void list_current_directory(void) {
    char cwd[MAX_PATH_LEN];
    DIR *dirp = opendir(getcwd(cwd, sizeof(cwd)));
    printf("heeeey")
    if (dirp == NULL) {
        perror("");  // prints why the open failed
        exit(1);
    }

    // directory entities
    struct dirent *dir;

    // if (cwd == NULL) {
    //     perror("");
    //     exit(1);
    // }


    // store all dir names in an array
    while ((dir = readdir(dirp)) != NULL) {
        struct stat s;

        // error checking
        if (stat(dir->d_name, &s) != 0) {
            fprintf(stderr, "Could not stat file\n");
            perror(dir->d_name);
            exit(1);
        }
        mode_t m = s.st_mode;
        print_perms(m);
        printf("\t%s\n", dir->d_name);
    }

    closedir(dirp);
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 1                    //
//                                          //
//////////////////////////////////////////////
bool test_can_encrypt(char filename[MAX_PATH_LEN]) {
    printf("TODO: COMPLETE ME\n");
    exit(1);
}

void simple_xor_encryption(char filename[MAX_PATH_LEN]) {
    if (!test_can_encrypt(filename)) return;

    printf("TODO: COMPLETE ME\n");
    exit(1);
}


void simple_xor_decryption(char filename[MAX_PATH_LEN]) {
    if (!test_can_encrypt(filename)) return;

    printf("TODO: COMPLETE ME\n");
    exit(1);
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 2                    //
//                                          //
//////////////////////////////////////////////
void search_by_filename(char filename[MAX_SEARCH_LENGTH]) {
    printf("TODO: COMPLETE ME\n");
    exit(1);
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


void search_by_filename(char filename[MAX_SEARCH_LENGTH]) {

    // error checking
    if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {

    }

    if (filename == NULL) {
        // fprintf(stdout, )
    }


    char cwd[MAX_PATH_LEN];
    getcwd(cwd, sizeof(cwd));

    // temporarily change to home dir so we can use previous cwd
    // with stat in recurseDirs
    char *home =  getenv("HOME");
    chdir(home);
    // start recursion from curr dir
    recurseDirs(cwd, filename);

    // change curr dir to original dir
    chdir(cwd);


}

// reads a files
void recurseText(char *src, char text[MAX_SEARCH_LENGTH]) {

    DIR *dirp = opendir (src);

    if (dirp == NULL) {
        printf("just ended\n");
        perror("");  // prints why the open failed
        return;
    }


    struct dirent *dir;
    struct text_find textStruct;
    struct text_find textArr[MAX_SEARCH_LENGTH];

    while ((dir = readdir(dirp)) != NULL) {
        struct stat s;

        // error checking
        if (stat(dir->d_name, &s) != 0) {
            fprintf(stdout, "%s", MSG_ERROR_FILE_STAT);
            perror(dir->d_name);
            return;
        }
        mode_t m = s.st_mode;


        //printf("i WORK AT LEAY");
        // its a file, read it and store all its contents into string

        if (S_ISREG(m)) {
            FILE *inputstream = fopen(dir->d_name, "r");
            if (inputstream == NULL) {
                perror("");
            }
            printf("%s\n", dir->d_name);
            // find how many chars in the file
            fseek(inputstream, 0, SEEK_END);
            int len = ftell(inputstream);
            // move back to beg of file
            fseek(inputstream, 0, SEEK_SET);
            char *string = malloc(sizeof(char) * (len + 1));

            // read in chars one at a
            int i = 0;
            int c;

            while ((c = fgetc(inputstream)) != EOF) {
                string[i] = c;
                printf("%c", string[i]);
                i++;
            }
            string[i] = '\0';
            printf("len %lu\n", strlen(string));
            // free(string);
        }
    }

    closedir(dirp);

}