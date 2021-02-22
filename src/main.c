#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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
    char* testText = "NOWTHISIS";
    flags = 0;
    fileCount = 0;
    curIn = 0;
    curOut = 0;
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
                fprintf(stdout, "fenc version: %.1f \n", VERSION_STRING);
                break;
            case 'h':
                fprintf(stderr, "(1) Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
                exit(EXIT_FAILURE);
            case 'p':
                flags += 5000;
                if(strcmp(optarg, "-") == 0){
                    curIn = 0;
                    fileCount++;
                    password = getpass("(1) Please provide a password for encryption/decryption: ");
                }
                break;
            case 'D':
                flags += 600000;
                debugString = optarg;
                if((debugString[strlen(debugString) - 1] != 'd')){
                    fprintf(stderr, "(2) Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                getDebugValue(debugString);
                break;
            case ':':
                switch (optopt)
                {
                case 'p':
                    password = getpass("(2) Please provide a password for encryption/decryption: ");
                    fprintf(stdout, "Password given: %s \n", password);
                    break;
                default:
                    fprintf(stderr, "(3) Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "(4) Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    fprintf(stderr, "optind : %i \n", optind);
    if(optind >= argc){
        fprintf(stderr, "No infile/outfile provided. Please try again. \n");
        fprintf(stderr, "Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while(optind < argc && fileCount < 2){
        if(fileCount == 0){
            if(strcmp(argv[optind], "-") != 0){
                curIn = open(argv[optind], O_RDONLY);
            }
            else{
                curIn = 0;
            }
        }
        else if(fileCount == 1){
            if(strcmp(argv[optind], "-") != 0){
                curOut = open(argv[optind], O_WRONLY);
            }
            else{
                curOut = 1;
            }
            break;
        }      
        fprintf(stderr, "extra arguments: %s\n", argv[optind]);
        fileCount++;
        optind++;  
    }

    int writeRet = write(curOut, testText, 9);
    fprintf(stderr, "\nwriteRet : %i with curOut: %i \n", writeRet, curOut);
    if(curIn != 0){
        close(curIn);
    }
    if(curOut != 1){
        close(curOut);
    }

    //  NOTE  If curIn or curOut are equal to zero, use stdin and stdout respectively. 
}