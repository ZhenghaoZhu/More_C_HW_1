#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "fenc.h"

int d00 = 0;
int d01 = 0;
int d02 = 0;
int d04 = 0;
int d16 = 0;
int d32 = 0;

char* pwdBuf = NULL;

/* 
 * open() -> read() -> open() -> write() -> close() (twice)
 */

void getDebugValue(char* debugString){
    int numberHolder = 0;
    int count = 0;
    for(int i = 0; i < strlen(debugString) - 1; i++){
        numberHolder *= 10;
        numberHolder += (int)(debugString[i] - 48);
    }
    d00 = numberHolder & 0;
    d01 = numberHolder & 1;
    d02 = numberHolder & 2;
    d04 = numberHolder & 4;
    d16 = numberHolder & 16;
    d32 = numberHolder & 32;
    printf("%i, %i, %i, %i, %i, %i \n", d00, d01, d02, d04, d16, d32);
}

int getFilePassword(char* curFile){
    pwdBuf = (char*)calloc(PWD_FILE_READ + 1, sizeof(char));
    char* tmpBuf = NULL;
    if(pwdBuf == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    int pwdLen = 0;
    int readRet = 0;
    int curFD = open(curFile, O_RDONLY);
    while((readRet = read(curFD, pwdBuf + pwdLen, PWD_FILE_READ)) > 0){
        pwdLen += readRet;
        if(readRet == PWD_FILE_READ){
            tmpBuf = (char*)realloc(pwdBuf, pwdLen + PWD_FILE_READ);
            if(tmpBuf != NULL){
                pwdBuf = tmpBuf;
            }
            else {
                perror("realloc");
                exit(EXIT_FAILURE);
            }
        }
    }
    pwdBuf[pwdLen] = '\0';
    return 1;
}

int copyPassword(char* password){
    pwdBuf = strdup(password);
    if(pwdBuf == NULL){
        return 0;
    }
        fprintf(stderr, "Given password : %s \n", pwdBuf);
    return 1;
}

int readIn(){
    return 1;
}

int writeOut(){
    return 1;
}

int closeAll(){
    if(fdOut >= 0){
        close(fdIn);
    }
    if(fdOut >= 0){
        close(fdOut);
    }
    if(pwdBuf != NULL){
        fprintf(stderr, "Password before free: %s \n", pwdBuf);
        free(pwdBuf);
    }
    return 1;
}

