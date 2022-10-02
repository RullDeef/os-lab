#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include <unistd.h>
#include <string.h> // strcmp
#include <stdbool.h>

#define __USE_MISC
#include <dirent.h>

#include "stack.h"


int walk_dirp(DIR *dp);

bool dot_or_2dot(char* dname);
void print_dirent(int ident, struct dirent* dent);
void print_dirend(int ident);

int main(int argc, const char* argv[]) {
    int status = EXIT_FAILURE;
    const char *dirpath;
    DIR *dp;

    dirpath = argc == 1 ? "." : argv[1];

    chdir(dirpath);

    if ((dp = opendir(".")) == NULL)
        perror("opendir");
    else {
        status = walk_dirp(dp);
        closedir(dp);
    }

    return status;
}

int walk_dirp(DIR *dp) {
    int status = EXIT_SUCCESS;
    struct dirent *dent;
    struct stat statbuf;

    stack_t* ent_stk = NULL;
    int ident = 0;

    do {
        while (status == EXIT_SUCCESS && (dent = readdir(dp)) != NULL && errno == 0) {
            if (lstat(dent->d_name, &statbuf) == -1) {
                perror("lstat");
                status = EXIT_FAILURE;
            } else if (!dot_or_2dot(dent->d_name)) {
                print_dirent(ident, dent);

                if (S_ISDIR(statbuf.st_mode)) {
                    stk_push(&ent_stk, dp);
                    dp = opendir(dent->d_name);
                    ident++;

                    chdir(dent->d_name);
                }
            }
        }

        if (ent_stk != NULL) {
            print_dirend(ident);
            closedir(dp);
            chdir("..");
            dp = stk_pop(&ent_stk);
            ident--;
        }
    } while (ent_stk != NULL);

    if (errno != 0) {
        perror("readdir");
        status = EXIT_FAILURE;
    }

    return status;
}

bool dot_or_2dot(char* dname) {
    return strcmp(dname, ".") == 0 || strcmp(dname, "..") == 0;
}

void print_dirent(int ident, struct dirent* dent) {
    printf("%8ld ", dent->d_ino);

    for (int i = 0; i < ident; i++)
        printf("│ ");

    printf("%s\n", dent->d_name);
}

void print_dirend(int ident) {
    printf("         ");

    for (int i = 0; i < ident - 1; i++)
        printf("│ ");
    
    printf("└\n");
}
