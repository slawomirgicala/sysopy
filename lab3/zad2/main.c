#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <time.h>
#include <unistd.h>


char* read_file(char* file_path) {
    struct stat status;
    if (lstat(file_path, &status) != 0){
        printf("Cannot read file status \n");
        exit(1);
    }

    FILE* file = fopen(file_path, "r");
    if (file == NULL){
        printf("Cannot read file");
        exit(1);
    }

    char* buffer = malloc(status.st_size);

    if (fread(buffer, 1, status.st_size, file) != status.st_size){
        printf("Cannot read file");
        exit(1);
    }

    fclose(file);

    return buffer;
}




void monitor1(char* file_path, unsigned int period, unsigned int time){

    int passed = 0;
    int copies_number = 0;
    struct stat status;

    if (lstat(file_path, &status) == -1){
        printf("Error with getting file status");
        exit(0);
    }

    char* file_name = basename(file_path);

    char* file_name_date = malloc(strlen(file_name) + 50);
    char* date = malloc(25);

    time_t last_mod = status.st_mtime;
    char* old_version = read_file(file_path);

    while (passed < time){

        sleep(period);

        if (lstat(file_path, &status) == -1){
            printf("Error with getting file status");
            exit(0);
        }

        if (last_mod < status.st_mtime){

            strftime(date, 25, "%Y-%m-%d_%H-%M-%S", localtime(&status.st_mtime));

            sprintf(file_name_date, "./archive/%s_%s", file_name, date);

            FILE *copy = fopen(file_name_date, "w");

            fwrite(old_version, 1, strlen(old_version), copy);

            fclose(copy);

            free(old_version);

            old_version = read_file(file_path);
            last_mod = status.st_mtime;
            copies_number++;
        }
        passed += period;
    }
    free(file_name_date);
    free(date);
    free(old_version);

    exit(copies_number);
}


void monitor2(char* file_path, unsigned int period, unsigned int time){

    int passed = 0;
    int copies_number = 0;
    struct stat status;

    if (lstat(file_path, &status) == -1){
        printf("Error with getting file status");
        exit(0);
    }

    char* file_name = basename(file_path);

    char* file_name_date = malloc(strlen(file_name) + 50);
    char* date = malloc(25);

    time_t last_mod = status.st_mtime;
    //char* old_version = read_file(file_path);

    while (passed < time){

        sleep(period);

        if (lstat(file_path, &status) == -1){
            printf("Error with getting file status");
            exit(0);
        }

        if (last_mod < status.st_mtime){

            strftime(date, 25, "%Y-%m-%d_%H-%M-%S", localtime(&status.st_mtime));

            sprintf(file_name_date, "./archive/%s_%s", file_name, date);

            pid_t child_pid = fork();
            if (child_pid == 0) {
                execlp("cp", "cp", file_path, file_name_date, NULL);
                printf("Oooops! Execlp did not work :O\n");
                exit(1);
            } else if (child_pid == -1) {
                printf("Copy did not work '%s'\n", file_name);
            } else {
                int status;
                wait(&status);
                if (WIFEXITED(status)){
                    copies_number++;
                } else{
                    printf("Process cp %d did not terminate normally\n", (int) child_pid);
                }
            }

            last_mod = status.st_mtime;
        }
        passed += period;
    }
    free(file_name_date);
    free(date);

    exit(copies_number);
}









int monitor(char* file_path, unsigned int period, unsigned int time, int mode) {
    pid_t child_pid = fork();

    if (child_pid == 0) {
        if (mode == 1)
            monitor1(file_path, period, time);
        else
            monitor2(file_path, period, time);
    } else if (child_pid == -1) {
        printf("Error with starting process for: %s\n", file_path);
        return 0;
    }
    return 1;
}



int main(int argc, char* argv[]){

    if (argc != 4){
        printf("Wrong number of args! Give list, time and mode\n");
        exit(1);
    }

    char* list = argv[1];

    unsigned int time = (unsigned int) strtol(argv[2],NULL,10);

    char* mode = argv[3];

    if (strcmp(mode, "1") == 0){
        struct stat st;
        if (stat("./archive", &st) == -1) {
            if (mkdir("./archive", 0777) != 0){
                printf("Cannot make archive\n");
                exit(1);
            }
        }
    }

    FILE* file_stream = fopen(list,"r");

    if (file_stream == NULL){
        printf("Cannot read file :c");
        exit(1);
    }

    char* line = NULL;
    size_t length = 0;
    ssize_t nread;

    char* file_path;
    char* period;

    int children_number = 0;

    while ((nread = getline(&line, &length, file_stream)) != -1) {
        if ((file_path = strsep(&line, " ")) == NULL){
                printf("Error with parsing file\n");
                exit(1);
        }
        if ((period = strsep(&line, " ")) == NULL){
                printf("Error with parsing file\n");
                exit(1);
        }

        unsigned int period_n = (unsigned int) strtoul(period, NULL, 10);

        if (strcmp(mode, "1") == 0){
            children_number += monitor(file_path, period_n, time, 1);
        } else if (strcmp(mode, "2") == 0){
            children_number += monitor(file_path, period_n, time, 2);
        } else{
            printf("Wrong mode! 1 - write, 2 - copy with cp\n");
            exit(1);
        }
    }
    free(line);
    fclose(file_stream);

    for (int i = 0; i < children_number; i++){
        int status;
        pid_t child_pid = wait(&status);
        if (WIFEXITED(status)) {
            printf("Process %d made %d copies\n", (int) child_pid, WEXITSTATUS(status));
        } else {
            printf("Process %d did not terminate normally\n", (int) child_pid);
        }
    }

    return 0;
}