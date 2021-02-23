#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statfs.h> 
#include <fcntl.h>
#include <errno.h>
#include "fenc.h"

//+++++++++++++++++++++++++++++++++//
void exitWithFailure();
void checkStatMode(mode_t statMode);
//+++++++++++++++++++++++++++++++++//

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
                if(getFilePassword(optarg) == 1){
                    exitWithFailure();
                }
                break;
            case 'D':
                opt_D += 1;
                debugString = optarg;
                if((debugString[strlen(debugString) - 1] != 'd')){
                    exitWithFailure();
                }
                if(getDebugValue(debugString) == 1){
                    exitWithFailure();
                }
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
            exitWithFailure();
        }
        free(password);
    }

    struct stat fileStat;
    while(optind < argc){
        if(fileCount == 0){
            if(strcmp(argv[optind], "-") != 0){
                if(stat(argv[optind], &fileStat) == -1){
                    perror("stat");
                    exitWithFailure();
                }
                checkStatMode(fileStat.st_mode);
                if((access(argv[optind], F_OK) != -1) && (access(argv[optind], R_OK) == 0)){
                    fdIn = open(argv[optind], O_RDONLY);
                }
                else {
                    perror("access (Read)");
                    exitWithFailure();
                }
            }
            else {
                fdIn = 0;
            }
        }
        else if(fileCount == 1){
            if(strcmp(argv[optind], "-") != 0){
                if(stat(argv[optind], &fileStat) == -1){
                    perror("stat");
                    exitWithFailure();
                }
                checkStatMode(fileStat.st_mode);
                if((access(argv[optind], F_OK) != -1) && (access(argv[optind], W_OK) == 0)){
                    //  TODO  Use mkstemp
                    fdOut = open(argv[optind], O_WRONLY);
                }
                else {
                    //  TODO  make one 
                    perror("access (Write)");
                    exitWithFailure();
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

    if(closeAll() == 1){
        fprintf(stderr, "Usage: ./fenc [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n");
        exit(EXIT_FAILURE);
    }

}

void exitWithFailure(void){
    if(closeAll() == 1){
        fprintf(stderr, "Usage: ./fenc [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Usage: ./fenc [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n");
    exit(EXIT_FAILURE);
}

void checkStatMode(mode_t statMode){
    if(statMode & S_IFDIR){
        perror("stat (directory, not file)");
        exitWithFailure();
    }
    if(statMode & S_IFCHR){
        perror("stat (character device, not file)");
        exitWithFailure();
    }
    if(statMode & S_IFBLK){
        perror("stat (block device, not file)");
        exitWithFailure();
    }
    return;
}