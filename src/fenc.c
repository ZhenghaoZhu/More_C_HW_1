#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <stdarg.h>
#include "fenc.h"

static int debugNone = 0;
static int debugFuncCalls = 0;
static int debugLibCalls = 0;
static int debugSysCalls = 0;
static int debugArgs = 0;
static int debugRet = 0;

static char* pwdBuf = NULL;
static char* shaPwdBuf = NULL;
static char* readInBuf = NULL;

int getDebugValue(char* debugString){
    int numberHolder = 0;
    int count = 0;
    int debugStrLen = my_strlen(debugString);
    for(int i = 0; i < debugStrLen - 1; i++){
        numberHolder *= 10;
        numberHolder += (int)(debugString[i] - 48);
    }
    debugNone = numberHolder & 0;
    debugFuncCalls = numberHolder & 1;
    debugLibCalls = numberHolder & 2;
    debugSysCalls = numberHolder & 4;
    debugArgs = numberHolder & 16;
    debugRet = numberHolder & 32;
    if((numberHolder > 0) && (debugArgs || debugRet) && !(debugFuncCalls || debugLibCalls || debugSysCalls)){
        my_fprintf("Provided debug value missing function, library function, or system call choice. Please try again \n");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", debugString);
        DBG_RET("%i", 1);
        return 1;
    }
    if((numberHolder > 0) && ((debugNone + debugFuncCalls + debugLibCalls + debugSysCalls + debugArgs + debugRet) == 0)){
        my_fprintf("Non-valid debug number provided, please try again \n");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", debugString);
        DBG_RET("%i", 1);
        return 1;
    }
    DBG_ORI_FN_CALLS("Entered", 1, "%s", debugString);
    DBG_ORI_FN_CALLS("Exited", 0, "%s", debugString);
    DBG_RET("%i", 0);
    return 0;
}

int getFilePassword(char* curFile){
    DBG_ORI_FN_CALLS("Entered", 1, "%s", curFile);
    pwdBuf = (char*)my_calloc(PWD_FILE_READ + 1, sizeof(char));
    char* tmpBuf = NULL;
    if(pwdBuf == NULL){
        my_perror("malloc");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", curFile);
        DBG_RET("%i", 1);
        return 1;
    }
    int pwdLen = 0;
    int readRet = 0;
    int curFD = my_open(curFile, O_RDONLY);

    if(curFD == -1){
        my_perror("open");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", curFile);
        DBG_RET("%i", 1);
        return 1;
    }

    while((readRet = my_read(curFD, pwdBuf + pwdLen, PWD_FILE_READ)) > 0){
        pwdLen += readRet;
        if(readRet == PWD_FILE_READ){
            tmpBuf = (char*)my_realloc(pwdBuf, pwdLen + PWD_FILE_READ);
            if(tmpBuf != NULL){
                pwdBuf = tmpBuf;
            }
            else {
                my_perror("realloc");
                DBG_ORI_FN_CALLS("Exited", 0, "%s", curFile);
                if(my_close(curFD) == -1){
                    my_perror("close");
                    DBG_ORI_FN_CALLS("Exited", 0, "%s", curFile);
                    DBG_RET("%i", 1);
                    return 1;
                }
                DBG_RET("%i", 1);
                return 1;
            }
        }
        else {
            break;
        }
    }

    if(my_close(curFD) == -1){
        my_perror("close");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", curFile);
        DBG_RET("%i", 1);
        return 1;
    }

    if(readRet < 0){
        my_perror("read");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", curFile);
        DBG_RET("%i", 1);
        return 1;
    }

    int actualLen = 1; // Start with 1 for null terminator
    while(actualLen <= pwdLen && pwdBuf[actualLen] != '\n'){
        actualLen++;
    }
    
    pwdBuf[actualLen] = '\0';
    tmpBuf = my_strdup(pwdBuf);
    my_free(pwdBuf);
    pwdBuf = tmpBuf;
    DBG_ORI_FN_CALLS("Exited", 0, "%s", curFile);
    DBG_RET("%i", 0);
    return 0;
}

int stdInPassword(int second_time){
    DBG_ORI_FN_CALLS("Entered", 1, "%s", "(void)");
    char* pwdBufSecond = NULL;
    pwdBuf = getpass("Please provide password used to decrypt/encrypt: ");
    if(pwdBuf == NULL){
        my_fprintf("Unable to save password, please try again \n");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", "(void)");
        DBG_RET("%i", 1);
        return 1;
    }

    if(second_time){
        char* tmpBuf = strdup(pwdBuf);
        pwdBufSecond = getpass("Please confirm password by retyping it: ");
        if(strcmp(tmpBuf, pwdBufSecond) != 0){
            my_fprintf("Passwords don't match. Please try again \n");
            my_free(tmpBuf);
            return 1;
        }
        my_free(tmpBuf);
    }

    DBG_ORI_FN_CALLS("Exited", 0, "%s", "(void)");
    DBG_RET("%i", 0);
    return 0;
}

int copyStart(char* fdInPath, char* fdOutPath, int opt_e, int opt_i){
    DBG_ORI_FN_CALLS("Entered", 1, "%s %s %i", fdInPath, fdOutPath, opt_e);
    int pageSize = my_getpagesize();
    char* tmpBuf = NULL;
    char tmpFilePath[] = ".test_XXXXXX";
    int tempFdOut = 0;
    int fdIn = 0;
    readInBuf = (char*)my_calloc(pageSize + 1, sizeof(char));
    tempFdOut = my_mkstemp(tmpFilePath);
    if(readInBuf == NULL){
        my_perror("malloc");
        DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
        DBG_RET("%i", 1);
        return 1;
    }
    if(tempFdOut == -1){
        my_perror("mkstemp");
        DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
        DBG_RET("%i", 1);
        return 1;
    }
    if(fdInPath != NULL){
        fdIn = my_open(fdInPath, O_RDONLY);
        if(fdIn < 0){
            my_perror("open");
            DBG_RET("%i", 1);
            return 1;
        }
    }
    int curReadRet = 0;
    int readRet = 0;
    int writeRet = 0;
    char* computedHashOptE = NULL;
    char* computedHash = NULL;
    char* givenHash = NULL;
    if(opt_e){ // Encrypting
        char hashBuf[65];
        computedHashOptE = (char*)my_calloc(65, sizeof(char));
        strToSha256(pwdBuf, hashBuf);
        strncat(computedHashOptE, &hashBuf[0], HASH_LENGTH);
        writeRet = my_write(tempFdOut, computedHashOptE, HASH_LENGTH);
        if(writeRet != HASH_LENGTH){
            my_perror("write");
            goto closeEverything;
        }
    }
    else{ // Decrypting
        char hashBuf[65];
        computedHash = (char*)my_calloc(65, sizeof(char));
        givenHash = (char*)my_calloc(65, sizeof(char));
        strToSha256(pwdBuf, hashBuf);
        strncat(computedHash, &hashBuf[0], HASH_LENGTH);
        readRet = my_read(fdIn, givenHash, HASH_LENGTH);
        if(readRet < HASH_LENGTH || strcmp(givenHash, computedHash) != 0){
            my_fprintf("Provided password doesn't match with hash in file, please try again \n");
            goto closeEverything;
        }
        if(readRet != HASH_LENGTH){
            my_perror("read");
            goto closeEverything;
        }
    }
    
    int pageCounter = 0;
    while((readRet = my_read(fdIn, readInBuf, pageSize)) > 0){
        curReadRet = readRet;
        if(readRet == pageSize){
            if(opt_e == 1){
                #ifdef EXTRA_CREDIT
                if(opt_i){
                    pageCounter++;
                }
                writeRet = encryptBuf(tempFdOut, (unsigned char*)readInBuf, (const unsigned char*)computedHashOptE, readRet, pageCounter);
                #else
                writeRet = encryptBuf(tempFdOut, (unsigned char*)readInBuf, (const unsigned char*)computedHashOptE, readRet, pageCounter);
                #endif
            }
            else{
                #ifdef EXTRA_CREDIT
                if(opt_i){
                    pageCounter++;
                }
                writeRet = decryptBuf(tempFdOut, (unsigned char*)readInBuf, (const unsigned char*)computedHash, readRet, pageCounter);
                #else
                writeRet = decryptBuf(tempFdOut, (unsigned char*)readInBuf, (const unsigned char*)computedHash, readRet, pageCounter);
                #endif
                
            }
            
            if(getenv("BAD_WRITE") != NULL){
                my_fprintf("Injected error to write \n");
                errno = EIO;
                writeRet = -1;
            }
            if(writeRet == 1 || writeRet == -1){
                my_perror("write");
                closeEverything:
                my_remove(tmpFilePath);
                if(my_close(fdIn) == -1){
                    my_perror("close");
                    DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
                    DBG_RET("%i", 1);
                    if(computedHash != NULL){
                        my_free(computedHash);
                    }
                    if(givenHash != NULL){
                        my_free(givenHash);
                    }
                    if(computedHashOptE != NULL){
                        my_free(computedHashOptE);
                    }
                    return 1;
                }
                DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
                DBG_RET("%i", 1);
                if(computedHash != NULL){
                    my_free(computedHash);
                }
                if(givenHash != NULL){
                    my_free(givenHash);
                }
                if(computedHashOptE != NULL){
                    my_free(computedHashOptE);
                }
                return 1;
            }
        }
        else {
            break;
        }
    }
    
    if(readRet < 0){
        my_perror("read");
        my_remove(tmpFilePath);
        DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
        DBG_RET("%i", 1);
        return 1;
    }

    if(my_close(fdIn) == -1){
        my_perror("close");
        my_remove(tmpFilePath);
        DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
        DBG_RET("%i", 1);
        return 1;
    }

    readInBuf[curReadRet] = '\0';
    if(opt_e == 1){
        writeRet = encryptBuf(tempFdOut, (unsigned char*)readInBuf, (const unsigned char*)pwdBuf, readRet, pageCounter);
    }
    else{
        writeRet = decryptBuf(tempFdOut, (unsigned char*)readInBuf, (const unsigned char*)pwdBuf, readRet, pageCounter);
    }

    if(writeRet == 1){
        my_fprintf("Writing to outfile failed, please try again \n");
        my_remove(tmpFilePath);
        DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
        DBG_RET("%i", 1);
        return 1;
    }

    if(my_close(tempFdOut) == -1){
        my_perror("close");
        my_remove(tmpFilePath);
        DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
        DBG_RET("%i", 1);
        return 1;
    }

    if(my_remove(fdOutPath) == -1){
        my_perror("remove");
        my_remove(tmpFilePath);
        DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
        DBG_RET("%i", 1);
        return 1;
    }
    
    if(my_rename(tmpFilePath, fdOutPath) == -1){
        my_perror("rename");
        my_remove(tmpFilePath);
        DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
        DBG_RET("%i", 1);
        return 1;
    }

    if(computedHash != NULL){
        my_free(computedHash);
    }
    if(givenHash != NULL){
        my_free(givenHash);
    }
    if(computedHashOptE != NULL){
        my_free(computedHashOptE);
    }

    DBG_ORI_FN_CALLS("Exited", 0, "%s %s %i", fdInPath, fdOutPath, opt_e);
    DBG_RET("%i", 0);
    return 0;
}


/************************************************************/
/*                    CRYPTO FUNCTIONS                      */
/************************************************************/

void strToSha256(char* curStr, char hashBuf[65]){
    DBG_ORI_FN_CALLS("Entered", 1, "%s %s", curStr, "hashBuf");
    unsigned char hash32Container[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, curStr, strlen(curStr));
    SHA256_Final(hash32Container, &sha256);
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        sprintf(hashBuf + (i * 2), "%02x", hash32Container[i]);
    }
    hashBuf[64] = 0;
    DBG_ORI_FN_CALLS("Exited", 0, "%s %s", curStr, "hashBuf");
}

int encryptBuf(int curFdOut, unsigned char* curBuf, const unsigned char *pwdKey, int curBufLen, int pageCounter){
    DBG_ORI_FN_CALLS("Entered", 1, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
    int cipherBufLen = 0;
    unsigned char iv[16] = {pageCounter};
    unsigned char* cipherBuf;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if(ctx == NULL){
        my_fprintf("CTX init failed, please try again \n");
        DBG_ORI_FN_CALLS("Exited", 0, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
        DBG_RET("%i", 1);
        return 1;
    }
    EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, pwdKey, iv);
    cipherBuf = (unsigned char*)my_calloc(curBufLen, sizeof(char));
    if(EVP_EncryptUpdate(ctx, cipherBuf, &cipherBufLen, curBuf, curBufLen) != 1){
        my_fprintf("EVP_DecryptUpdate failed, please try again \n");
        my_free(cipherBuf);
        EVP_CIPHER_CTX_free(ctx);
        DBG_RET("%i", 1);
        DBG_ORI_FN_CALLS("Exited", 0, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
        return 1;
    }
    int writeRet = write(curFdOut, cipherBuf, cipherBufLen);
    if(writeRet != cipherBufLen){
        my_fprintf("Writing encrypted to file failed, please try again \n");
        my_free(cipherBuf);
        EVP_CIPHER_CTX_free(ctx);
        DBG_ORI_FN_CALLS("Exited", 0, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
        DBG_RET("%i", 1);
        return 1;
    }
    my_free(cipherBuf);
    EVP_CIPHER_CTX_free(ctx);
    DBG_ORI_FN_CALLS("Exited", 0, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
    DBG_RET("%i", 0);
    return 0;
}

int decryptBuf(int curFdOut, unsigned char* curBuf, const unsigned char *pwdKey,int curBufLen, int pageCounter){
    DBG_ORI_FN_CALLS("Entered", 1, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
    int cipherBufLen = 0;
    unsigned char iv[16] = {pageCounter};
    unsigned char* cipherBuf;
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if(ctx == NULL){
        my_fprintf("CTX init failed, please try again \n");
        DBG_RET("%i", 1);
        DBG_ORI_FN_CALLS("Exited", 0, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
        return 1;
    }
    if(EVP_DecryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, pwdKey, iv) != 1){
        my_fprintf("EVP_DecryptInit_ex failed, please try again \n");
        EVP_CIPHER_CTX_free(ctx);
        DBG_RET("%i", 1);
        DBG_ORI_FN_CALLS("Exited", 0, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
        return 1;
    }
    cipherBuf = (unsigned char*)my_calloc(curBufLen, sizeof(char));
    if(EVP_DecryptUpdate(ctx, cipherBuf, &cipherBufLen, curBuf, curBufLen) != 1){
        my_fprintf("EVP_DecryptUpdate failed, please try again \n");
        my_free(cipherBuf);
        EVP_CIPHER_CTX_free(ctx);
        DBG_RET("%i", 1);
        DBG_ORI_FN_CALLS("Exited", 0, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
        return 1;
    }
    int writeRet = write(curFdOut, cipherBuf, cipherBufLen);
    if(writeRet != cipherBufLen){
        my_fprintf("Writing encrypted to file failed, please try again \n");
        my_free(cipherBuf);
        EVP_CIPHER_CTX_free(ctx);
        DBG_RET("%i", 1);
        DBG_ORI_FN_CALLS("Exited", 0, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
        return 1;
    }
    my_free(cipherBuf);
    EVP_CIPHER_CTX_free(ctx);
    DBG_RET("%i", 1);
    DBG_ORI_FN_CALLS("Exited", 0, "%i %s %s %i", curFdOut, curBuf, pwdKey, curBufLen);
    return 0;
}

int closeAll(){
    DBG_ORI_FN_CALLS("Entered", 1, "%s", "(void)");
    if(fdInPath != NULL){
        my_free(fdInPath);
    }
    if(fdOutPath != NULL){
        my_free(fdOutPath);
    }
    if(pwdBuf != NULL){
        my_free(pwdBuf);
    }

    if(readInBuf != NULL){
        my_free(readInBuf);
    }
    DBG_ORI_FN_CALLS("Exited", 0, "%s", "(void)");
    DBG_RET("%i", 0);
    return 0;
}


/************************************************************/
/*                    DEBUG FUNCTIONS                       */
/************************************************************/

int getDebugFuncCalls(){
    return debugFuncCalls;
}

int getDebugLibCalls(){
    return debugLibCalls;
}

int getDebugSysCalls(){
    return debugSysCalls;
}

int getDebugArgs(){
    return debugArgs;
}

int getDebugRet(){
    return debugRet;
}

int my_getopt(int n, char** arr, char* string){
    DBG_LIB_FN_CALLS("Entered", 1, "%i %p %s", n, arr, string);
    int ret = getopt(n, arr, string);
    DBG_LIB_FN_CALLS("Exited", 0, "%i %p %s", n, arr, string);
    DBG_RET("%i", ret);
    return ret;
}

int my_fprintf(char* output){
    DBG_LIB_FN_CALLS("Entered", 1, "%s", output);
    int ret = fprintf(stderr, "%s", output);
    DBG_LIB_FN_CALLS("Exited", 0, "%s", output);
    DBG_RET("%i", ret);
    return ret;
}

void my_perror(char* output){
    DBG_LIB_FN_CALLS("Entered", 1, "%s", output);
    perror(output);
    DBG_LIB_FN_CALLS("Exited", 0, "%s", output);
    DBG_RET("%s", "(void)");
    return;
}

int my_strcmp(char* c1, char* c2){
    DBG_LIB_FN_CALLS("Entered", 1, "%s %s", c1, c2);
    int ret = strcmp(c1, c2);
    DBG_LIB_FN_CALLS("Exited", 0, "%s %s", c1, c2);
    DBG_RET("%i", ret);
    return ret;
}

int my_access(char* name, int type){
    DBG_LIB_FN_CALLS("Entered", 1, "%s %i", name, type);
    int ret = access(name, type);
    DBG_LIB_FN_CALLS("Exited", 0, "%s %i", name, type);
    DBG_RET("%i", ret);
    return ret;
}

int my_rename(char* old, char* new){
    DBG_LIB_FN_CALLS("Entered", 1, "%s %s", old, new);
    int ret = rename(old, new);
    DBG_LIB_FN_CALLS("Exited", 0, "%s %s", old, new);
    DBG_RET("%i", ret);
    return ret;
}

int my_stat(char* file, struct stat* buf){
    DBG_LIB_FN_CALLS("Entered", 1, "%s %p", file, buf);
    int ret = stat(file, buf);  
    DBG_LIB_FN_CALLS("Exited", 0, "%s %p", file, buf);
    DBG_RET("%i", ret);
    return ret;
}

char* my_strdup(char* string){
    DBG_LIB_FN_CALLS("Entered", 1, "%s", string);
    char* ret = strdup(string);  
    DBG_LIB_FN_CALLS("Exited", 0, "%s", string);
    DBG_RET("%s", ret);
    return ret;
}

int my_strlen(char* string){
    DBG_LIB_FN_CALLS("Entered", 1, "%s", string);
    int ret = strlen(string); 
    DBG_LIB_FN_CALLS("Exited", 0, "%s", string);
    DBG_RET("%i", ret);
    return ret;
}

long my_sysconf(int name){
    DBG_LIB_FN_CALLS("Entered", 1, "%i", name);
    long ret = sysconf(name); 
    DBG_LIB_FN_CALLS("Exited", 0, "%i", name);
    DBG_RET("%li", ret);
    return ret;
}

long my_getpagesize(){
    DBG_LIB_FN_CALLS("Entered", 1, "%s", "(void)");
    int ret = getpagesize(); 
    DBG_LIB_FN_CALLS("Exited", 0, "%s", "(void)");
    DBG_RET("%i", ret);
    return ret;
}

void* my_calloc(size_t nmemb, size_t size){
    DBG_LIB_FN_CALLS("Entered", 1, "%zu %zu", nmemb, size);
    void* ret = calloc(nmemb, size); 
    DBG_LIB_FN_CALLS("Exited", 0, "%zu %zu", nmemb, size);
    DBG_RET("%p", ret);
    return ret;
}

void* my_realloc(void *ptr, size_t size){
    DBG_LIB_FN_CALLS("Entered", 1, "%p %zu", ptr, size);
    void* ret = realloc(ptr, size); 
    DBG_LIB_FN_CALLS("Exited", 0, "%p %zu", ptr, size);
    DBG_RET("%p", ret);
    return ret;
}

void my_free(void *ptr){
    DBG_LIB_FN_CALLS("Entered", 1, "%p", ptr);
    free(ptr);
    DBG_LIB_FN_CALLS("Exited", 0, "%p", ptr);
    DBG_RET("%s", "(void)");
    return;
}

int my_mkstemp(char* name){
    DBG_LIB_FN_CALLS("Entered", 1, "%s", name);
    int ret = mkstemp(name); 
    DBG_LIB_FN_CALLS("Exited", 0, "%s", name);
    DBG_RET("%i", ret);
    return ret;
}

int my_remove(char* name){
    DBG_LIB_FN_CALLS("Entered", 1, "%s", name);
    int ret = remove(name); 
    DBG_LIB_FN_CALLS("Exited", 0, "%s", name);
    DBG_RET("%i", ret);
    return ret;
}

int my_open(char* name, int flag){
    DBG_SYS_FN_CALLS("Entered", 1, "%s %i", name, flag);
    int ret = open(name, flag); 
    DBG_SYS_FN_CALLS("Exited", 0, "%s %i", name, flag);
    DBG_RET("%i", ret);
    return ret;
}

int my_close(int fd){
    DBG_SYS_FN_CALLS("Entered", 1, "%i", fd);
    int ret = close(fd);
    DBG_SYS_FN_CALLS("Exited", 0, "%i", fd);
    DBG_RET("%i", ret);
    return ret;
}

int my_read(int fd, char* buf, int nbytes){
    DBG_SYS_FN_CALLS("Entered", 1, "%i %s %i", fd, buf, nbytes);
    int ret = read(fd, buf, nbytes); 
    DBG_SYS_FN_CALLS("Exited", 0, "%i %s %i", fd, buf, nbytes);
    DBG_RET("%i", ret);
    return ret;
}

int my_write(int fd, char* buf, int nbytes){
    DBG_SYS_FN_CALLS("Entered", 1, "%i %s %i", fd, buf, nbytes);
    int ret = write(fd, buf, nbytes); 
    DBG_SYS_FN_CALLS("Exited", 0, "%i %s %i", fd, buf, nbytes);
    DBG_RET("%i", ret);
    return ret;
}

void my_open_create(char* enterOrExit, int firstCall, char* argvOpt, int create, int irw){
    DBG_SYS_FN_CALLS(enterOrExit, firstCall, "%s %i %i", argvOpt, create, irw);
}
