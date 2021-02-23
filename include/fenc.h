#define VERSION_STRING 1.0
#define PWD_FILE_READ 8

int fdIn;
int fdOut;

int getDebugValue(char* debugString);
int getFilePassword(char* curFile);
int copyPassword(char* password);
int readIn();
int writeOut();
int closeAll();

void debugFunctionCallsArgs(char* curFunction, char* curArgs);
void debugLibCallsArgs(char* curFunction, char* curArgs);
void debugSysCallsArgs(char* curFunction, char* curArgs);