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

static char* pwdBuf = NULL;
static char* readInBuf = NULL;

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

    if(close(curFD) == -1){
        perror("close");
        return 1;
    }

    if(readRet < 0){
        perror("read");
        return 1;
    }

    int actualLen = 1; // Start with 1 for null terminator
    while(actualLen <= pwdLen && pwdBuf[actualLen] != '\n'){
        actualLen++;
    }
    
    pwdBuf[actualLen] = '\0';
    return 0;
}

int stdInPassword(){
    pwdBuf = getpass("Please provide password used to decrypt/encrypt: ");
    if(pwdBuf == NULL){
        fprintf(stderr, "Unable to save password, please try again \n");
        return 1;
    }
        fprintf(stderr, "Given password : %s \n", pwdBuf);
    return 0;
}

int copyStart(char* fdInPath, char* fdOutPath, int opt_e){
    fprintf(stderr, "In copyProcess() \n");
    int pageSize = getpagesize();
    char* tmpBuf = NULL;
    char tmpFilePath[] = "test_XXXXXX";
    int tempFdOut = 0;
    int fdIn = 0;
    readInBuf = (char*)calloc(pageSize + 1, sizeof(char));
    tempFdOut = mkstemp(tmpFilePath);
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
    int curReadRet = 0;
    int readRet = 0;
    int writeRet = 0;
    while((readRet = read(fdIn, readInBuf, pageSize)) > 0){
        curReadRet = readRet;
        if(readRet == pageSize){
            //  TODO  Encrypt/Decrypt with password
            writeRet = write(tempFdOut, readInBuf, curReadRet);
            if(writeRet < 0 || writeRet < curReadRet || writeRet > curReadRet){
                perror("write");
                remove(tmpFilePath);
                return 1;
            }
        }
        else {
            break;
        }
    }

    if(readRet < 0){
        perror("read");
        remove(tmpFilePath);
        return 1;
    }

    if(close(fdIn) == -1){
        perror("close");
        remove(tmpFilePath);
        return 1;
    }

    readInBuf[curReadRet] = '\0';
    //  TODO  Encrypt/Decrypt with password
    writeRet = write(tempFdOut, readInBuf, curReadRet);

    if(writeRet < 0 || writeRet < curReadRet || writeRet > curReadRet){
        perror("write");
        remove(tmpFilePath);
        return 1;
    }

    if(close(tempFdOut) == -1){
        perror("close");
        remove(tmpFilePath);
        return 1;
    }

    if(remove(fdOutPath) == -1){
        perror("remove");
        remove(tmpFilePath);
        return 1;
    }
    
    if(rename(tmpFilePath, fdOutPath) == -1){
        perror("rename");
        remove(tmpFilePath);
        return 1;
    }

    return 0;
}

int encryptBuf(int curFd, char* curBuf, int curBufLen){
    //  TODO  
    return 0;
}

int decryptBuf(){
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

