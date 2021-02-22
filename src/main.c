#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include "fenc.h"

/* 
 * -d : Decrypt infile into outfile  TODO 
 * -e : Encrypt encrypt infile into outfile  TODO 
 * -v : Print version string of the program  DONE 
 * -h : The program should print a simple usage line on stderr and exit with a non-zero status code  DONE 
 * -p : Supports ARG as the name of a file that contains the password as the first line of the file.
 * -D : Debugging information printed to stdeer based on DBGVAL
 * ● Debug value 0x00 (0d): no debug info printed
 * ● Debug value 0x01 (1d): print a message on immediate entry and right before the exit to every function
 *   in your code that you write, including main(), but not library calls you call. Print the name of the function
 *   and whether you're entering or exiting it.
 * ● Debug value 0x02 (2d): print right before and right after calling any library call (e.g., from libc, libssl, 
 *   etc.). Print the function name and whether you're before or after calling it. 
 * ● Debug value 0x04 (4d): print right before and right after calling any system call (e.g., open, read, write, 
 *   close). Print the syscall name and whether you're before or after calling it. 
 * ● Debug value 0x10 (16d): print also arguments to any function for debug values 0x1 (upon entry), 0x2 
 *   (right before), and 0x4 (right before).
 * ● Debug value 0x20 (32d): print also return values (and errors if any) for any function for debug values 
 *   0x1 (right before return), 0x2 (right after), and 0x4 (right after).
 */
int main(int argc, char *argv[], char *envp[]) {
    int flags, opt, fileCount;
    char* password  = NULL;
    flags = 0;
    fileCount = 0;
    curIn = NULL;
    curOut = NULL;
    char* debugString = NULL;
    while ((opt = getopt(argc, argv, ":devhp:D:")) != -1) {
        switch (opt) {
            case 'd':
                flags += 1;
                break;
            case 'e':
                flags += 20;
                break;
            case 'v': 
                flags += 300;
                printf("fenc version: %.1f \n", VERSION_STRING);
                break;
            case 'h':
                fprintf(stderr, "Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
                exit(EXIT_FAILURE);
            case 'p':
                flags += 5000;
                break;
            case 'D':
                flags += 600000;
                debugString = optarg;
                if((debugString[strlen(debugString) - 1] != 'd')){
                    fprintf(stderr, "Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                getDebugValue(debugString);
                break;
            case ':':
                switch (optopt)
                {
                case 'p':
                    printf("option -%c with no file provided\n", optopt);
                    password = getpass("Please provide a password for encryption/decryption: ");
                    printf("Password given: %s \n", password);
                    break;
                default:
                    fprintf(stderr, "Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    while(optind < argc){
        if(fileCount == 0){
            if(strcmp(argv[optind], "-") == 0){
                curIn = argv[optind];
            }
            else{
                // FIXME  Should be a FILE*
                curIn = "stdin";
            }
        }
        else if(fileCount == 1){
            if(strcmp(argv[optind], "-") == 0){
                curOut = argv[optind];
            }
            else{
                // FIXME  Should be a FILE*
                curOut = "stdout";
            }
            break;
        }      
        printf("extra arguments: %s\n", argv[optind]);
        fileCount++;
        optind++;  
    }

    if(curIn == NULL){
        //  TODO  What to set as curIn
        // curIn = stdin;
    }
    if(curOut == NULL){
        //  TODO  What to set as curIn
        // curOut = stdout;
    }
}