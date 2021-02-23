#define VERSION_STRING 1.0
#define PWD_FILE_READ 8

int fdIn;
int fdOut;

void getDebugValue(char* debugString);
int getFilePassword(char* curFile);
int copyPassword(char* password);
int readIn();
int writeOut();
int closeAll();