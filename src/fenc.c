#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "fenc.h"

static int d00 = 0;
static int d01 = 0;
static int d02 = 0;
static int d04 = 0;
static int d16 = 0;
static int d32 = 0;

char* pwdBuf = NULL;
char* readInBuf = NULL;

/* 
 * open() -> read() -> open() -> write() -> close() (twice)
 */

int getDebugValue(char* debugString){
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
    if((numberHolder > 0) && ((d00 + d01 + d02 + d04 + d16 + d32) == 0)){
        fprintf(stderr, "Non-valid debug number provided, please try again \n");
        return 1;
    }
    return 0;
    // printf("%i, %i, %i, %i, %i, %i \n", d00, d01, d02, d04, d16, d32);
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

    if(curFD == -1){
        perror("open");
        return 1;
    }

    while((readRet = read(curFD, pwdBuf + pwdLen, PWD_FILE_READ)) > 0){
        pwdLen += readRet;
        if(readRet == PWD_FILE_READ){
            tmpBuf = (char*)realloc(pwdBuf, pwdLen + PWD_FILE_READ);
            if(tmpBuf != NULL){
                pwdBuf = tmpBuf;
            }
            else {
                perror("realloc");
                return 1;
            }
        }
        else {
            break;
        }
    }

    if(readRet < 0){
        perror("read");
        return 1;
    }

    pwdBuf[pwdLen] = '\0';
    return 0;
}

int copyPassword(char* password){
    pwdBuf = strdup(password);
    if(pwdBuf == NULL){
        fprintf(stderr, "Unable to save password, please try again \n");
        return 1;
    }
        fprintf(stderr, "Given password : %s \n", pwdBuf);
    return 0;
}

int copyProcess(char* fdInPath, char* fdOutPath, int opt_e){
    int pageSize = getpagesize();
    char* tmpBuf = NULL;
    char tmpFile[] = "test_XXXXXX";
    int tempFdOut = 0;
    int fdIn = 0;
    readInBuf = (char*)calloc(pageSize + 1, sizeof(char));
    tempFdOut = mkstemp(tmpFile);
    if(readInBuf == NULL){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if(tempFdOut == -1){
        perror("mkstemp");
        exit(EXIT_FAILURE);
    }
    if(fdInPath != NULL){
        fdIn = open(fdInPath, O_RDONLY);
    }
    int fileLen = 0;
    int readRet = 0;
    int writeRet = 0;
    while((readRet = read(fdIn, readInBuf, pageSize)) > 0){
        fileLen = readRet;
        if(readRet == pageSize){
            //  TODO  Encrypt/Decrypt with password
            writeRet = write(tempFdOut, readInBuf, fileLen);
            if(writeRet < 0 || writeRet < fileLen || writeRet > fileLen){
                perror("write");
                return 1;
            }
        }
        else {
            break;
        }
    }

    if(readRet < 0){
        perror("read");
        return 1;
    }

    readInBuf[fileLen] = '\0';
    fprintf(stderr, "fileLen: %i \n", fileLen);
    //  TODO  Encrypt/Decrypt with password
    writeRet = write(tempFdOut, readInBuf, fileLen);

    if(writeRet < 0 || writeRet < fileLen || writeRet > fileLen){
        perror("write");
        return 1;
    }

    if(remove(fdOutPath) == -1){
        perror("remove");
        return 1;
    }
    
    if(rename(tmpFile, fdOutPath) == -1){
        perror("rename");
        return 1;
    }

    return 0;
}

int encryptCopy(){
    //  TODO  
    return 0;
}

int decryptCopy(){
    //  TODO  
    return 0;
}

int closeAll(){
    if(fdInPath != NULL){
        free(fdInPath);
    }
    if(fdOutPath != NULL){
        free(fdOutPath);
    }
    if(pwdBuf != NULL){
        fprintf(stderr, "Password before free: %s \n", pwdBuf);
        free(pwdBuf);
    }

    if(readInBuf != NULL){
        fprintf(stderr, "Freeing the readInBuf \n");
        free(readInBuf);
    }
    return 0;
}


/************************************************************/
/*                    DEBUG FUNCTIONS                       */
/************************************************************/

void debugFunctionCallsArgs(char* curFunction, char* curArgs){
    if(d01 == 0){
        return;
    }
}

void debugLibCallsArgs(char* curFunction, char* curArgs){
    if(d02 == 0){
        return;
    }
}

void debugSysCallsArgs(char* curFunction, char* curArgs){
    if(d04 == 0){
        return;
    }
}

