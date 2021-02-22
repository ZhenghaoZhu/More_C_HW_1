#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fenc.h"

int d00 = 0;
int d01 = 0;
int d02 = 0;
int d04 = 0;
int d16 = 0;
int d32 = 0;

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

int readIn(){
    return 1;
}

int writeOut(){
    return 1;
}

