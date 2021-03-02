#include <openssl/evp.h>

#define VERSION_STRING "Fenc version 1.0 \n"
#define PWD_FILE_READ 8
#define HASH_LENGTH 64
// #define EXTRA_CREDIT 1

#define DBG_ORI_FN_CALLS(enteredOrExit, firstCall, FORMAT, ...) \
    if (getDebugFuncCalls()) { \
        fprintf(stderr, "%s original function %s() in line %i, File: %s \n", enteredOrExit, __func__, __LINE__, __FILE__); \
    } \
    if (getDebugArgs() && firstCall) { \
        fprintf(stderr, "Function %s ", __func__); \
        fprintf(stderr, "args: " FORMAT "\n", __VA_ARGS__); \
    } \


#define DBG_LIB_FN_CALLS(enteredOrExit, firstCall, FORMAT, ...) \
    if (getDebugArgs() && firstCall) { \
        fprintf(stderr, "Function %s ", __func__); \
        fprintf(stderr, "args: " FORMAT "\n", __VA_ARGS__); \
    } \
    if (getDebugLibCalls()) { \
        fprintf(stderr, "%s library function %s() in line %i, File: %s  \n", enteredOrExit, __func__, __LINE__, __FILE__); \
    } \

#define DBG_SYS_FN_CALLS(enteredOrExit, firstCall, FORMAT, ...) \
    if (getDebugArgs() && firstCall) { \
        fprintf(stderr, "Function %s ", __func__); \
        fprintf(stderr, "args: " FORMAT "\n", __VA_ARGS__); \
    } \
    if (getDebugSysCalls()) { \
        fprintf(stderr, "%s system call %s() in line %i, File: %s  \n", enteredOrExit, __func__, __LINE__, __FILE__); \
    } \

#define DBG_RET(FORMAT, ...) \
    if (getDebugRet()) { \
        fprintf(stderr, "Function %s ", __func__); \
        fprintf(stderr, "returns: " FORMAT "\n", __VA_ARGS__); \
    } \

char* fdInPath;
char* fdOutPath;

int getDebugValue(char* debugString);
int getFilePassword(char* curFile);
int stdInPassword(int second_time);
int copyStart(char* fdInPath, char* fdOutPath, int opt_e, int opt_i);
void strToSha256(char* curStr, char hashBuf[65]);
int initCTX(EVP_CIPHER_CTX *ctx, int do_encrypt);
int encryptBuf(int curFdOut, unsigned char* curBuf, const unsigned char *pwdKey,int curBufLen, int pageCounter);
int decryptBuf(int curFdOut, unsigned char* curBuf, const unsigned char *pwdKey,int curBufLen, int pageCounter);
int closeAll();

int getDebugFuncCalls();
int getDebugLibCalls();
int getDebugSysCalls();
int getDebugArgs();
int getDebugRet();
int my_getopt(int n, char** arr, char* string);
int my_fprintf(char* output);
void my_perror(char* output);
int my_strcmp(char* c1, char* c2);
int my_access(char* name, int type);
int my_rename(char* old, char* new);
int my_stat(char* file, struct stat* buf);
char* my_strdup(char* string);
int my_strlen(char* string);
long my_sysconf(int name);
long my_getpagesize();
void* my_calloc(size_t nmemb, size_t size);
void* my_realloc(void *ptr, size_t size);
void my_free(void* ptr);
int my_mkstemp(char* name);
int my_remove(char* name);
int my_open(char* name, int flag);
int my_close(int fd);
int my_read(int fd, char* buf, int nbytes);
int my_write(int fd, char* buf, int nbytes);
void my_open_create(char* enterOrExit, int firstCall, char* argvOpt, int create, int irw);