#define _XOPEN_SOURCE 500
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ftw.h>
#include <limits.h>
#include <sys/wait.h>




void dirs_ls(const char* absolute_path, const char* relative_path){
    pid_t child_pid = fork();

    if (child_pid == -1) {
        printf("Oh dear, something went wrong with fork()! %s\n", strerror(errno));
        exit(1);
    } else if (child_pid == 0) {
        printf("\nDIR: %s\nPID: %d\n", relative_path, (int) getpid());
        execlp("ls", "ls", "-l", absolute_path, NULL);
    } else {
        int status;
        wait(&status);
        if (!WIFEXITED(status)) {
            printf("ERROR: Child has not terminated correctly.\n");
            exit(1);
        }
    }
}

void search_dir(const char* absolute_path, const char* relative_path){

    dirs_ls(absolute_path, relative_path);

    DIR* dir = opendir(absolute_path);
    if (dir == NULL){
        printf("Cannot open directory\n");
        exit(1);
    }

    struct dirent* entry;

    struct stat status;

    while ((entry = readdir(dir)) != NULL){

        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){

            char* new_relative_path = malloc(strlen(relative_path) + strlen(entry->d_name) + 2);
            char* new_absolute_path = malloc(strlen(absolute_path) + strlen(entry->d_name) + 2);

            if (new_relative_path == NULL || new_absolute_path == NULL){
                printf("Cannot allocate memory");
                exit(1);
            }

            sprintf(new_absolute_path, "%s/%s", absolute_path, entry->d_name);

            if (strcmp(relative_path, ".") == 0) {
                sprintf(new_relative_path, "%s", entry->d_name);
            } else {
                sprintf(new_relative_path, "%s/%s", relative_path, entry->d_name);
            }

            if (lstat(new_absolute_path, &status) == -1){
                printf("Oh dear, something went wrong with lstat! %s\n", strerror(errno));
                exit(1);
            }


            if (S_ISDIR(status.st_mode)){
                search_dir(new_absolute_path, new_relative_path);
            }

            free(new_absolute_path);
            free(new_relative_path);
        }
    }
    closedir(dir);
}


static int nftw_util(const char *path, const struct stat *stat, int typeflag, struct FTW *ftwbuf) {
    if (ftwbuf->level == 0){
        dirs_ls(path,path);
        return 0;
    }

    if (S_ISDIR(stat->st_mode)){
        dirs_ls(path,path);
        return 0;
    }

    return 0;
}


int main(int argc, char* argv[]){

    if (argc != 3){
        printf("Wrong args");
        exit(1);
    }

    char* absolute_path = realpath(argv[1], NULL);

    if (absolute_path == NULL){
        printf("Cannot get absolute path!\n");
        exit(1);
    }

    char* mode = argv[2];

    if (strcmp(mode, "1") == 0){
        search_dir(absolute_path, ".");
    } else{
        nftw(absolute_path, nftw_util, 10, FTW_PHYS);
    }

    free(absolute_path);
    return 0;
}