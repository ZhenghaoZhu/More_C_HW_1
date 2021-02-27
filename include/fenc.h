#define VERSION_STRING 1.0
#define PWD_FILE_READ 8

char* fdInPath;
char* fdOutPath;

int getDebugValue(char* debugString);
int getFilePassword(char* curFile);
int copyPassword(char* password);
int copyProcess(char* fdInPath, char* fdOutPath, int opt_e);
int encryptCopy();
int decryptCopy();
int closeAll();

void debugFunctionCallsArgs(char* curFunction, char* curArgs);
void debugLibCallsArgs(char* curFunction, char* curArgs);
void debugSysCallsArgs(char* curFunction, char* curArgs);