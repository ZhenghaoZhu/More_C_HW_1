#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "fenc.h"


void exitWithFailure();
/* 
 * -d : Decrypt infile into outfile  TODO 
 * -e : Encrypt encrypt infile into outfile  TODO 
 * -v : Print version string of the program  DONE 
 * -h : The program should print a simple usage line on stderr and exit with a non-zero status code  DONE 
 * -p : Supports ARG as the name of a file that contains the password as the first line of the file  DONE  
 * -D : Debugging information printed to stdeer based on DBGVAL  TODO 
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
    int opt_d = 0, opt_e = 0, opt_v = 0, opt_p = 0, opt_D = 0;
    int opt, fileCount = 0;
    char* password = NULL;
    fdIn = 0;
    fdOut = 0;
    char* debugString = NULL;
    while ((opt = getopt(argc, argv, ":devhp:D:")) != -1) {
        switch (opt) {
            case 'd':
                opt_d += 1;
                break;
            case 'e':
                opt_e += 1;
                break;
            case 'v': 
                opt_v += 1;
                if(opt_v == 1){
                    fprintf(stdout, "fenc version: %.1f \n", VERSION_STRING);
                }
                break;
            case 'h':
                fprintf(stderr, "(1) Usage: %s [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n", argv[0]);
                exit(EXIT_FAILURE);
            case 'p':
                opt_p += 1;
                getFilePassword(optarg);
                break;
            case 'D':
                opt_D += 1;
                debugString = optarg;
                if((debugString[strlen(debugString) - 1] != 'd')){
                    exitWithFailure();
                }
                getDebugValue(debugString);
                break;
            default:
                exitWithFailure();
        }
    }


    if(optind >= argc){
        fprintf(stderr, "No infile/outfile provided. Please try again. \n");
        exitWithFailure();
    }

    if((argc - optind) > 2){
        fprintf(stderr, "Too many arguments provided please try again. \n");
        exitWithFailure();
    }

    if(opt_d >= 1 && opt_e >= 1){
        fprintf(stderr, "You cannot have both -d and -e, please try again. \n");
        exitWithFailure();
    }

    if(opt_d == 0 && opt_e == 0){
        fprintf(stderr, "You need to provide either -e or -d, please try again. \n");
        exitWithFailure();
    }

    if(opt_d > 1 || opt_e > 1 || opt_v > 1 || opt_p > 1 || opt_D > 1){
        fprintf(stderr, "You cannot have multiple of the same flag, please try again. \n");
        exitWithFailure();
    }

    if(opt_p == 0){
        password = getpass("Please provide a password to encrypt/decrypt: ");
        if(copyPassword(password) != 1){
            perror("copyPassword");
            closeAll();
            exit(EXIT_FAILURE);
        }
        free(password);
    }

    while(optind < argc){
        if(fileCount == 0){
            if(strcmp(argv[optind], "-") != 0){
                if(access(argv[optind], R_OK) == 0){
                    fdIn = open(argv[optind], O_RDONLY);
                }
                else {
                    perror(strerror(errno));
                    exitWithFailure();
                }
            }
            else {
                fdIn = 0;
            }
        }
        else if(fileCount == 1){
            if(strcmp(argv[optind], "-") != 0){
                if(access(argv[optind], W_OK) == 0){
                    //  TODO  Use mkstemp
                    fdOut = open(argv[optind], O_WRONLY);
                }
                else {
                    //  TODO  make one 
                    fprintf(stderr, "OUTFILE DOESNT EXIST \n");
                }
            }
            else {
                fdOut = 1;
            }
        }
        fileCount++;
        optind++;  
    }


    fprintf(stderr, "curIn : %i, curOut: %i \n", fdIn, fdOut);

    closeAll();

}

void exitWithFailure(void){
    closeAll();
    fprintf(stderr, "Usage: ./fenc [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n");
    exit(EXIT_FAILURE);
}