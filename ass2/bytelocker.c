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
int getOccurences (char *word, char *string);
int recurseText(char *src, char text[MAX_SEARCH_LENGTH], int index, struct text_find *structArr, char *relativeP);
int recurseDirs(char* src, char substring[MAX_SEARCH_LENGTH], char *arr[], int index);

void print_perms(mode_t m);
void printName(char* src, char *name);
void padFile (char filename[MAX_PATH_LEN], int bytes);

uint8_t cyclicalLeftShift(int n_rotations, uint8_t bits);
uint8_t cyclicalRightShift(int n_rotations, uint8_t bits);


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
        fprintf(stdout, "Something went wrong\n");
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

    // change directory
    if (chdir(dest_directory) != 0) fprintf(stderr, MSG_ERROR_CHANGE_DIR);

    if (strcmp(dest_directory, "~") == 0) {
        // get the home dir
        char *home =  getenv("HOME");
        chdir(home);
        printf("Moving to %s\n", home);
        return;
    }

    printf("Moving to %s\n", dest_directory);
}

//
//  Lists the contents of the current directory to stdout.
//
void list_current_directory(void) {

    char cwd[MAX_PATH_LEN];
    DIR *dirp = opendir(getcwd(cwd, sizeof(cwd)));

    if (dirp == NULL) {
        fprintf(stdout, "Cannot open directory\n");
        return;
    }

    struct dirent *dir;

    // store all dir names in an array
    int i = 0;
    char *name[MAX_NUM_FINDS];

    // store dirs and file names into a list
    while ((dir = readdir(dirp)) != NULL) {
        name[i] =  malloc (sizeof(char) * strlen(dir->d_name)+1);
        strncpy (name[i],dir->d_name, strlen(dir->d_name) + 1);
        i++;
    }

    int len = i;
    sort_by_name(name, len);

    for (i = 0; i < len; i++) {
        struct stat s;

        if (stat(name[i], &s) != 0) {
            fprintf(stdout,"%s", MSG_ERROR_FILE_STAT);
            return;
        }
        mode_t m = s.st_mode;
        print_perms(m);
        printf("\t%s\n", name[i]);

    }

    // free name[]
    i = 0;
    while ((dir = readdir(dirp)) != NULL) {
        free(name[i]);
        i++;
    }
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
        return res;
    }

    mode_t m = s.st_mode;

    // must be a file
    if (!S_ISREG(m)) {
        fprintf(stdout, MSG_ERROR_DIRECTORY, filename);
        return res;
    }

    // group must be able to read file
    if (!(m & S_IRGRP)) {
        fprintf(stdout, MSG_ERROR_READ, filename);
        return res;
    }

    // group must have perm to write to file
    if (!(m & S_IWGRP)) {
        fprintf(stdout, MSG_ERROR_WRITE, filename);
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
        fprintf(stdout, MSG_ERROR_FILE_OPEN);
        return;
    }

    if (outputStream == NULL) {
        fprintf(stdout, MSG_ERROR_FILE_OPEN);
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
        fprintf(stdout, MSG_ERROR_FILE_OPEN);
        return;
    }

    if (outputStream == NULL) {
        fprintf(stdout, MSG_ERROR_FILE_OPEN);
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


// goes through all files and dirs and reads the files

void search_by_filename(char filename[MAX_SEARCH_LENGTH]) {

    // error checking
    if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
        fprintf(stdout, MSG_ERROR_RESERVED);
    }

    if (strlen(filename) == 0) fprintf(stdout, MSG_ERROR_SEARCH);

    char cwd[MAX_PATH_LEN];
    getcwd(cwd, sizeof(cwd));

    // temporarily change to home dir so we can use previous cwd with stat in recurseDirs
    char *home =  getenv("HOME");
    chdir(home);

    char **arr;
    arr = malloc(MAX_NUM_FINDS * sizeof(char*));

    for (int i = 0; i < MAX_NUM_FINDS; i++) arr[i] = malloc(sizeof(char) * MAX_DIR_NAME);
    int numNames = recurseDirs(cwd, filename, arr, 0);

    // sort and print out file name and its perms
    sort_by_name(arr, numNames);
    for (int i = 0; i < numNames; i++) printName(cwd, arr[i]);

    // change curr dir to original dir
    chdir(cwd);

    for (int i = 0; i < MAX_NUM_FINDS; i++) free(arr[i]);
    free(arr);

}


void search_by_text(char text[MAX_SEARCH_LENGTH]) {

    if (strlen(text) == 0) fprintf(stdout, MSG_ERROR_SEARCH);

    char cwd[MAX_PATH_LEN];
    getcwd(cwd, sizeof(cwd));

    // temporarily change to home dir so we can use previous cwd with stat in recurseDirs
    char *home =  getenv("HOME");
    chdir(home);

    // start recursion from curr dir
    struct text_find structArr[MAX_SEARCH_LENGTH];

    char relativeP[MAX_PATH_LEN] = ".";
    int len = recurseText(cwd, text, 0, structArr, relativeP);

    sort_by_count(structArr, len);
    for (int i = 0; i < len; i++) {
        if (structArr[i].count > 0) {
            printf("%d:\t%s\n", structArr[i].count, structArr[i].path);
        }
    }

    chdir(cwd);
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 3                    //
//                                          //
//////////////////////////////////////////////


void electronic_codebook_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;

    //create filename.ecb name
    char ECBAppend[MAX_PATH_LEN] = ".ecb";
    char ECBFile[MAX_PATH_LEN];
    strcpy(ECBFile, filename);
    strcat(ECBFile, ECBAppend);

    FILE *inputStream = fopen(filename, "r");

    //checks
    if (inputStream == NULL) {
        fprintf(stdout, MSG_ERROR_FILE_OPEN);
        return;
    }
    // count the number of bytes, if not a multiple of 16, then pad it
    int c;
    int bytes = 0;
    while ((c = fgetc(inputStream)) != EOF) {
        bytes++;
    }

    padFile(filename, bytes);
    fseek(inputStream, 0, SEEK_SET);

    bytes = 0;
    while ((c = fgetc(inputStream)) != EOF) {
        bytes++;
    }

    FILE *outputStream = fopen(ECBFile, "w");

    if (outputStream == NULL) {
        fprintf(stdout, MSG_ERROR_FILE_OPEN);
        return;
    }

    int i = 0;

    fseek(inputStream, 0, SEEK_SET);

    // for every 16 bytes, write to a char array, pass that in to encrypt
    int counter = 0;
    char string[MAX_LINE_LEN];

    while (i < 16 && counter < bytes) {
        string[i] = fgetc(inputStream);
        if (i == 15) {
            fwrite(shift_encrypt(string, password), sizeof(char), RAND_STR_LEN, outputStream);
            i = 0;
            continue;
        }
        counter ++;
        i++;
    }

    fclose(inputStream);
    fclose(outputStream);
}

void electronic_codebook_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;

    //create filename.dec name
    char decAppend[MAX_PATH_LEN] = ".dec";
    char decFile[MAX_PATH_LEN];
    strcpy(decFile, filename);
    strcat(decFile, decAppend);

    FILE *inputStream = fopen(filename, "r");

    //checks
    if (inputStream == NULL) {
        fprintf(stdout, MSG_ERROR_FILE_OPEN);
        return;
    }
    // count the number of bytes, if not a multiple of 16, then pad it (just incase for decrp)
    int c;
    int bytes = 0;
    while ((c = fgetc(inputStream)) != EOF) {
        bytes++;
    }

    padFile(filename, bytes);
    fseek(inputStream, 0, SEEK_SET);

    bytes = 0;
    while ((c = fgetc(inputStream)) != EOF) {
        bytes++;
    }

    FILE *outputStream = fopen(decFile, "w");

    if (outputStream == NULL) {
        fprintf(stdout, MSG_ERROR_FILE_OPEN);
        return;
    }

    int i = 0;

    fseek(inputStream, 0, SEEK_SET);

    // for every 16 bytes, write to a char array, pass that in to encrypt
    int counter = 0;
    char string[MAX_LINE_LEN];

    while (i < 16 && counter < bytes) {
        string[i] = fgetc(inputStream);
        if (i == 15) {
            fwrite(shift_decrypt(string, password), sizeof(char), RAND_STR_LEN, outputStream);
            i = 0;
            continue;
        }
        counter ++;
        i++;
    }

    fclose(inputStream);
    fclose(outputStream);
}

char *shift_encrypt(char *plaintext, char *password) {

    for (int i = 0; i < 16; i++) {
        int encrypt = password[i];
        u_int8_t temp = cyclicalLeftShift(encrypt, plaintext[i]);
        plaintext[i] = temp;
    }
    return plaintext;

}



char *shift_decrypt(char *ciphertext, char *password) {

    for (int i = 0; i < 16; i++) {
        int encrypt = password[i];
        u_int8_t temp = cyclicalRightShift(encrypt, ciphertext[i]);
        ciphertext[i] = temp;
    }
    return ciphertext;
}


//////////////////////////////////////////////
//                                          //
//              SUBSET 4                    //
//                                          //
//////////////////////////////////////////////
void cyclic_block_shift_encryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;


    // create cbc ecryption file
    char CBCAppend[MAX_PATH_LEN] = ".cbc";
    char CBCFile[MAX_PATH_LEN];
    strcpy(CBCFile, filename);
    strcat(CBCFile, CBCAppend);

    // open file
    FILE *inputStream = fopen(filename, "r");

    //error checking
    if (inputStream == NULL) {
        fprintf(stdout, "%s", MSG_ERROR_FILE_OPEN);
        return;
    }

    // count the number of bytes, if not a multiple of 16, then pad it
    int c;
    int bytes = 0;
    while ((c = fgetc(inputStream)) != EOF) bytes++;

    padFile(filename, bytes);
    fseek(inputStream, 0, SEEK_SET);

    // get new num of bytes
    bytes = 0;
    while ((c = fgetc(inputStream)) != EOF) bytes++;

    FILE *outputStream = fopen(CBCFile, "w");

    if (outputStream == NULL) {
        fprintf(stdout, "%s", MSG_ERROR_WRITE);
        return;
    }

    // XOR the first 16 characters of the file with the initialisation vector.
    int i = 0;
    int counter = 0;
    char string[RAND_STR_LEN];

    // generator an initialisation vector
    char *encryptedString;
    char *initialisationVector = generate_random_string(1);


    fseek(inputStream, 0, SEEK_SET);
    while (i < 16 && counter < bytes) {
        string[i] = fgetc(inputStream);
        // XOR the first 16 characters of the file with the initialisation vector.
        initialisationVector[i] = string[i] ^ initialisationVector[i];

        if (i == 15) {
            encryptedString = shift_encrypt(initialisationVector, password);
            fwrite(encryptedString, sizeof(char), RAND_STR_LEN, outputStream);
            i = 0;
            counter++;
            continue;
        }
        counter ++;
        i++;
    }

    fclose(inputStream);
    fclose(outputStream);
    free(initialisationVector);

}

void cyclic_block_shift_decryption(char filename[MAX_PATH_LEN], char password[CIPHER_BLOCK_SIZE + 1]) {
    if (!test_can_encrypt(filename)) return;

    // create dec decryption file
    char decAppend[MAX_PATH_LEN] = ".dec";
    char decFile[MAX_PATH_LEN];
    strcpy(decFile, filename);
    strcat(decFile, decAppend);

    // open file
    FILE *inputStream = fopen(filename, "r");

    //error checking
    if (inputStream == NULL) {
        fprintf(stdout, "%s", MSG_ERROR_FILE_OPEN);
        return;
    }

    // count the number of bytes, if not a multiple of 16, then pad it
    int c;
    int bytes = 0;
    while ((c = fgetc(inputStream)) != EOF) bytes++;

    padFile(filename, bytes);
    fseek(inputStream, 0, SEEK_SET);

    // get new num of bytes
    bytes = 0;
    while ((c = fgetc(inputStream)) != EOF) bytes++;

    FILE *outputStream = fopen(decFile, "w");

    if (outputStream == NULL) {
        fprintf(stdout, "%s", MSG_ERROR_WRITE);
        return;
    }

    // XOR the first 16 characters of the file with the initialisation vector.
    int i = 0;
    int counter = 0;
    char string[RAND_STR_LEN];

    // generator an initialisation vector
    char *decryptedString;
    char *initialisationVector = generate_random_string(1);

    fseek(inputStream, 0, SEEK_SET);
    while (i < 16 && counter < bytes) {
        string[i] = fgetc(inputStream);

        // XOR the first 16 characters of the file with the initialisation vector.
        if (i == 15) {
            decryptedString = shift_decrypt(string, password);
            for(int j = 0; j < RAND_STR_LEN; j++) {
                decryptedString[j] = decryptedString[j] ^ initialisationVector[j];
            }
            fwrite(decryptedString, sizeof(char), RAND_STR_LEN, outputStream);
            // get the previous 16 encrypted bytes for initliasation vector
            fseek(inputStream, -16, SEEK_CUR);
            for (int j = 0; j < RAND_STR_LEN; j++) {
                initialisationVector[j] = fgetc(inputStream);
            }
            i = 0;
            counter++;
            continue;
        }
        counter ++;
        i++;
    }

    fclose(inputStream);
    fclose(outputStream);
    free(initialisationVector);
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

// given a mode_t variable, prints out the permissions
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

// given a substring and a string, counts the occurences of the substring within the string
// algorithm sourced from https://www.youtube.com/watch?v=Zydf9p1VOGU
int getOccurences (char *substring, char *string) {
    int textLen = strlen(substring);
    int fileLen = strlen(string);

    int end = fileLen + 1 - textLen;
    int occurrences = 0;
    // loop through all chars in the string

    for (int i = 0; i < end; i++) {
        // check for substring
        bool match = true;
        for (int j = 0; j < textLen; j++) {

            if (substring[j] != string[i + j]) {
                // if char not matching, go to next file char
                match = false;
                break;
            }
        }
        if (match) occurrences++;
    }
    return occurrences;
}


// pads a file until its bytes are a multiple of 16
void padFile (char filename[MAX_PATH_LEN], int bytes) {
    if (bytes % 16 == 0) return;

    int pad = 16 - (bytes % 16);
    FILE *outputStream = fopen(filename, "a");
    for (int i = 0; i < pad; i++) fputc('\x00', outputStream);
    fclose(outputStream);
}


// returns the byte after its been rotated left n_rotations
// algorith recycled from test07
uint8_t cyclicalLeftShift(int n_rotations, uint8_t bits) {

    // ASCCI cant be negative so no need to account for negative rotations

    if (n_rotations >= 8) n_rotations = n_rotations % 8;

    for ( int i = 0; i < n_rotations; i++) {
        uint8_t mask = 1 << 7;

        // record the most important bit just incase theres a 1
        uint8_t temp = mask & bits;
        bits = bits << 1;
        // rotate the bit (most sig bit becomes the least sig bit)
        bits = bits | (temp >>7) ;
    }
    return bits;
}


// returns the byte after its been rotated right n_rotations
uint8_t cyclicalRightShift(int n_rotations, uint8_t bits) {

    if (n_rotations >= 8) n_rotations = n_rotations % 8;

    for ( int i = 0; i < n_rotations; i++) {
        uint8_t mask = 1;
        uint8_t temp = mask & bits;
        bits = bits >> 1;
        bits = bits | (temp << 7) ;
    }
    return bits;
}


// an expensive algorithm where given a name within an list of them, recursively goes through all subdirs and finds it
// once found prints out out the array of file/dir name and its permissions, and repeats
void printName(char* src, char *name) {

    DIR *dirp = opendir (src);

    if (dirp == NULL) {
        fprintf(stdout, "Can not open directory");
        return;
    }

    // in order to stat current directory name, muust chdir to the correct path
    chdir(src);

    struct dirent *dir;
    while ((dir = readdir(dirp)) != NULL) {
        struct stat s;

        // error checking
        if (stat(dir->d_name, &s) != 0) {
            fprintf(stdout, MSG_ERROR_FILE_STAT);
            return;
        }
        mode_t m = s.st_mode;

        if (strcmp(name, dir->d_name) == 0) {
            print_perms(m);
            printf("\t./%s\n", name);
        }

        // if its a directory, search it
        if (S_ISDIR(m)) {

            // ignore . & .. since reading dir picks up these
            char path[MAX_PATH_LEN];
            if (strcmp(dir->d_name, ".") == 0 ||
                strcmp(dir->d_name, "..") == 0) continue;

            snprintf(path, sizeof(path), "%s/%s", src, dir->d_name);
            printName(path, name);
            chdir(src);
        }
    }
    closedir(dirp);
}


// recursively looks through all subdirectories of the cwd, if a file or directory name contains the
// provided substring, said substring is added to an array of strings
// returns the length of the array of strings
int recurseDirs(char* src, char substring[MAX_SEARCH_LENGTH], char *arr[], int index) {

    DIR *dirp = opendir (src);

    if (dirp == NULL) {
        fprintf(stdout, "Can not open directory");
        return index;
    }

    // in order to stat current directory name, muust chdir to the correct path
    chdir(src);

    struct dirent *dir;
    while ((dir = readdir(dirp)) != NULL) {
        struct stat s;

        // error checking
        if (stat(dir->d_name, &s) != 0) {
            fprintf(stdout, MSG_ERROR_FILE_STAT);
            return index;
        }
        mode_t m = s.st_mode;

        // finds first occurrence of string
        char *ret = strstr(dir->d_name, substring);
        if (ret) {
            // store the name in array if there is a substring match
            strncpy (arr[index],dir->d_name, strlen(dir->d_name) + 1);
            index++;
        }

        // if its a directory, search it
        if (S_ISDIR(m)) {

            // ignore . & .. since reading dir picks up these
            char path[MAX_PATH_LEN];
            if (strcmp(dir->d_name, ".") == 0 ||
                strcmp(dir->d_name, "..") == 0) continue;


            // create the new dir path
            snprintf(path, sizeof(path), "%s/%s", src, dir->d_name);
            index = recurseDirs(path, substring, arr, index);
            // make sure to change BACK to the previous dir when recursive call exits
            chdir(src);
        }

    }

    closedir(dirp);
    return index;
}

// reads all files from the cwd and its sub directories, storing info about files that contain a txt word in a struct Arr
// returns an int representing the leng of the struct Arrya
int recurseText(char *src, char text[MAX_SEARCH_LENGTH], int index, struct text_find *structArr, char *relativeP) {

    DIR *dirp = opendir (src);

    if (dirp == NULL) {
        fprintf(stdout, "Cannot open Directory\n");
        return index;
    }

    chdir(src);
    struct dirent *dir;
    //struct text_find textStruct;

    while ((dir = readdir(dirp)) != NULL) {
        struct stat s;
        char cwd[MAX_PATH_LEN];
        getcwd(cwd, sizeof(cwd));

        // error checking
        if (stat(dir->d_name, &s) != 0) {
            fprintf(stdout, "%s", MSG_ERROR_FILE_STAT);
            return index;
        }
        mode_t m = s.st_mode;
        // its a file, read it and store all its contents into string

        if (strcmp(dir->d_name, ".") == 0 ||
            strcmp(dir->d_name, "..") == 0) continue;

        // if its a register, we want to stores it relative path and associated occurence in our array
        char newRelativeP[MAX_PATH_LEN];
        if (S_ISREG(m)) {

            FILE *inputstream = fopen(dir->d_name, "r");
            if (inputstream == NULL) {
                fprintf(stdout, MSG_ERROR_FILE_OPEN);
                return index;
            }

            // we want to store the entire contents of the file into a string
            // find how many chars in the file
            fseek(inputstream, 0, SEEK_END);
            int len = ftell(inputstream);
            fseek(inputstream, 0, SEEK_SET);
            char *string = malloc(sizeof(char) * (len + 1));

            // read in chars one at a time
            int i = 0;
            int c;
            while ((c = fgetc(inputstream)) != EOF) {
                string[i] = c;
                i++;
            }
            string[i] = '\0';
            snprintf(newRelativeP, MAX_PATH_LEN, "%s/%s", relativeP, dir->d_name);

            // store it in the array
            structArr[index].count = getOccurences(text, string);
            strcpy(structArr[index].path, newRelativeP);

            index++;
            free(string);
        } else {
            // if its a dir, recursely go into it
            char path[MAX_PATH_LEN];

            // create the new dir path
            snprintf(path, sizeof(path), "%s/%s", src, dir->d_name);
            snprintf(newRelativeP, MAX_PATH_LEN, "%s/%s", relativeP, dir->d_name);
            index = recurseText(path ,text, index, structArr, newRelativeP);
            chdir(src);
        }
    }
    closedir(dirp);
    return index;
}