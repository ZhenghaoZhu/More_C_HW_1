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
 * -D : Debugging information printed to stdeer based on DBGVAL  DONE 
 * ● Debug value 0x00 (0d): no debug info printed  DONE 
 * ● Debug value 0x01 (1d): print a message on immediate entry and right before the exit to every function  DONE 
 *   in your code that you write, including main(), but not library calls you call. Print the name of the function
 *   and whether you're entering or exiting it.  DONE 
 * ● Debug value 0x02 (2d): print right before and right after calling any library call (e.g., from libc, libssl, 
 *   etc.). Print the function name and whether you're before or after calling it.   DONE 
 * ● Debug value 0x04 (4d): print right before and right after calling any system call (e.g., open, read, write, 
 *   close). Print the syscall name and whether you're before or after calling it.   DONE 
 * ● Debug value 0x10 (16d): print also arguments to any function for debug values 0x1 (upon entry), 0x2 
 *   (right before), and 0x4 (right before).  DONE  
 * ● Debug value 0x20 (32d): print also return values (and errors if any) for any function for debug values 
 *   0x1 (right before return), 0x2 (right after), and 0x4 (right after).  DONE  
 */
int main(int argc, char *argv[], char *envp[]) {
    int opt_d = 0, opt_e = 0, opt_v = 0, opt_p = 0, opt_D = 0, opt_s = 0, opt_i = 0;
    int opt, fileCount = 0;
    char* password = NULL;
    fdInPath = NULL;
    fdOutPath = NULL;
    char* debugString = NULL;
    #ifdef EXTRA_CREDIT
    while ((opt = my_getopt(argc, argv, ":devhsip:D:")) != -1) {
    #else
    while ((opt = my_getopt(argc, argv, ":devhp:D:")) != -1) {
    #endif
        switch (opt) {
            #ifdef EXTRA_CREDIT
            case 's':
                opt_s += 1;
                break;
            case 'i':
                opt_i += 1;
                break;
            #endif
            case 'd':
                opt_d += 1;
                break;
            case 'e':
                opt_e += 1;
                break;
            case 'v': 
                opt_v += 1;
                if(opt_v == 1){
                    my_fprintf(VERSION_STRING);
                }
                break;
            case 'h':
                exitWithFailure();
            case 'p':
                opt_p += 1;
                if(getFilePassword(optarg) == 1){
                    exitWithFailure();
                }
                break;
            case 'D':
                opt_D += 1;
                debugString = optarg;
                char checkD = debugString[my_strlen(debugString) - 1];
                if((checkD != 'd')){
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

    DBG_ORI_FN_CALLS("Entered", 1, "%i %p %p", argc, argv, envp);


    if(optind >= argc){
        my_fprintf("No infile/outfile provided. Please try again. \n");
        exitWithFailure();
    }

    if((argc - optind) > 2){
        my_fprintf("Too many arguments provided please try again. \n");
        exitWithFailure();
    }

    if(opt_d >= 1 && opt_e >= 1){
        my_fprintf("You cannot have both -d and -e, please try again. \n");
        exitWithFailure();
    }

    if(opt_d == 0 && opt_e == 0){
        my_fprintf("You need to provide either -e or -d, please try again. \n");
        exitWithFailure();
    }

    if(opt_d > 1 || opt_e > 1 || opt_v > 1 || opt_p > 1 || opt_D > 1 || opt_s > 1 || opt_i > 1){
        my_fprintf("You cannot have multiple of the same flag, please try again. \n");
        exitWithFailure();
    }

    if(opt_p < 1){
        if(stdInPassword(opt_s) == 1){
            exitWithFailure();
        }
    }


    struct stat fdInfileStat;
    struct stat fdOutfileStat;
    while(optind < argc){
        if(fileCount == 0){
            if(my_strcmp(argv[optind], "-") != 0){
                if(my_stat(argv[optind], &fdInfileStat) == -1){
                    my_perror("stat");
                    exitWithFailure();
                }
                checkStatMode(fdInfileStat.st_mode);
                if((my_access(argv[optind], F_OK) != -1) && (my_access(argv[optind], R_OK) == 0)){
                    fdInPath = my_strdup(argv[optind]);
                    if(fdInPath == NULL){
                        my_perror("strdup");
                        exitWithFailure();
                    }
                    long testPages = my_sysconf(_SC_AVPHYS_PAGES);
                    long availableBytes = testPages * my_getpagesize();
                    if(fdInfileStat.st_size > availableBytes){
                        my_fprintf("Available memory not sufficient for file encryption/decryption.\n");
                        exitWithFailure();
                    }
                }
                else {
                    my_perror("access (Read)");
                    exitWithFailure();
                }
            }
            else {
                fdInPath = NULL;
            }
        }
        else if(fileCount == 1){
            if(strcmp(argv[optind], "-") != 0){
                if(stat(argv[optind], &fdOutfileStat) == -1){
                    my_open_create("Entered", 1, argv[optind], O_CREAT, S_IRWXU);
                    int openCreateRet = open(argv[optind], O_CREAT, S_IRWXU);
                    my_open_create("Exited", 0, argv[optind], O_CREAT, S_IRWXU);
                    if(openCreateRet == -1){
                        perror("open");
                        exitWithFailure();
                    }
                }
                checkStatMode(fdOutfileStat.st_mode);
                if(fdInPath != NULL && (fdInfileStat.st_ino == fdOutfileStat.st_ino)){
                    my_fprintf("Filepaths provided are the same, please provide filepaths to different files.\n");
                    exitWithFailure();
                }
                if((my_access(argv[optind], F_OK) != -1) && (my_access(argv[optind], W_OK) == 0)){
                    fdOutPath = strdup(argv[optind]);
                    if(fdOutPath == NULL){
                        my_perror("strdup");
                        exitWithFailure();
                    }
                }
                else {
                    my_perror("access (Write)");
                    exitWithFailure();
                }
            }
            else {
                fdOutPath = NULL;
            }
        }
        fileCount++;
        optind++;  
    }

    if(fileCount != 2){
        my_fprintf("No infile/outfile provided. Please try again. \n");
        exitWithFailure();
    }

    if(copyStart(fdInPath, fdOutPath, opt_e, opt_i) == 1){
        my_fprintf("copyStart failed, please try again \n");
        exitWithFailure();
    }

    if(closeAll() == 1){
        my_fprintf("Usage: ./fenc [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n");
        exit(EXIT_FAILURE);
    }
    DBG_ORI_FN_CALLS("Exited", 0, "%i %p %p", argc, argv, envp);
    DBG_RET("%i", 0);
    return 0;
}

void exitWithFailure(void){
    DBG_ORI_FN_CALLS("Entered", 1, "%s", "(void)");
    if(closeAll() == 1){
        #ifdef EXTRA_CREDIT
        my_fprintf("Usage: ./fenc [-devhs] [-D DBGVAL] [-p PASSFILE] infile outfile\n Trying to free and exit program failed. \n");
        #else
        my_fprintf("Usage: ./fenc [-devhs] [-D DBGVAL] [-p PASSFILE] infile outfile\n Trying to free and exit program failed. \n");
        #endif 
        DBG_ORI_FN_CALLS("Exited", 0, "%s", "(void)");
        DBG_RET("%s", "(void)");
        exit(EXIT_FAILURE);
    }
    #ifdef EXTRA_CREDIT
    my_fprintf("Usage: ./fenc [-devhs] [-D DBGVAL] [-p PASSFILE] infile outfile\n");
    #else
    my_fprintf("Usage: ./fenc [-devh] [-D DBGVAL] [-p PASSFILE] infile outfile\n");
    #endif    
    DBG_ORI_FN_CALLS("Exited", 0, "%s", "(void)");
    DBG_RET("%s", "(void)");
    exit(EXIT_FAILURE);
}

void checkStatMode(mode_t statMode){
    DBG_ORI_FN_CALLS("Entered", 1, "%s", "mode_t statMode");
    if(statMode & S_IFDIR){
        my_perror("stat (directory, not file)");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", "mode_t statMode");
        exitWithFailure();
    }
    if(statMode & S_IFCHR){
        my_perror("stat (character device, not file)");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", "mode_t statMode");
        exitWithFailure();
    }
    if(statMode & S_IFBLK){
        my_perror("stat (block device, not file)");
        DBG_ORI_FN_CALLS("Exited", 0, "%s", "mode_t statMode");
        exitWithFailure();
    }
    DBG_ORI_FN_CALLS("Exited", 0, "%s", "mode_t statMode");
    DBG_RET("%s", "(void)");
    return;
}