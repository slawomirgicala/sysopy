//#define _XOPEN_SOURCE 500
//#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>







//uibiuiubiu




#define TIME_FORMAT "%d.%m.%Y %H:%M:%S"


char* gop;
time_t gtime;

time_t parse_time(char* s) {
    struct tm date;
    char* res = strptime(s, TIME_FORMAT, &date);
    if (res == NULL || *res != '\0') {
        printf("Wrong date format");
        exit(1);
    }
    return mktime(&date);
}

int time_comparator(time_t file_time, char* op, time_t time) {
    if(strcmp(op, "=") == 0) {
        return file_time == time;
    } else if (strcmp(op, ">") == 0) {
        return file_time > time;
    } else if (strcmp(op, "<") == 0) {
        return file_time < time;
    } else {
        printf("Wrong comparision");
    }
    return 0;
}

void show_file(const char* path, const struct stat* stat) {

    printf("%s\t", path);

    if (S_ISREG(stat->st_mode))
        printf("zwykły plik\t");
    else if (S_ISDIR(stat->st_mode))
        printf("katalog\t");
    else if (S_ISCHR(stat->st_mode))
        printf("urządzenie znakowe\t");
    else if (S_ISBLK(stat->st_mode))
        printf("urzączenie blokowe\t");
    else if (S_ISFIFO(stat->st_mode))
        printf("potok nazwany\t");
    else if (S_ISLNK(stat->st_mode))
        printf("link symboliczny\t");
    else
        printf("soket\t");

    printf("%ld\t", stat->st_size);

    char* buffer = malloc(128);
    if (buffer == NULL){
        printf("Cannot allocate memory");
    }
    strftime(buffer, 30, TIME_FORMAT, localtime(&stat->st_atime));
    printf("%s\t", buffer);
    strftime(buffer, 30, TIME_FORMAT, localtime(&stat->st_mtime));
    printf("%s\n", buffer);
    free(buffer);
}


static int tree_nftw(const char *path, const struct stat *stat, int typeflag, struct FTW *ftwbuf) {
    if (ftwbuf->level == 0) return 0;
    if (!time_comparator(stat->st_mtime, gop, gtime)) return 0;

    show_file(path, stat);

    return 0;
}


void tree(char* path, char* op, time_t time) {

    DIR* dir;
    struct dirent* ent;
    struct stat stat;

    if (!(dir = opendir(path))){
        printf("Cannot open dir");
        exit(1);
    }
    while ((ent = readdir(dir))) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        char* new_path = malloc(strlen(path) + strlen(ent->d_name) + 2);
        if (new_path == NULL){
            printf("cannot allocate memory");
            exit(1);
        }
        sprintf(new_path, "%s/%s", path, ent->d_name);

        if (lstat(new_path, &stat) == -1){
            printf("Errr");
            exit(1);
        }

        if (time_comparator(stat.st_mtime, op, time))
            show_file(new_path, &stat);

        if (S_ISDIR(stat.st_mode)) {
            tree(new_path, op, time);
        }
        free(new_path);
    }

    closedir(dir);
}






int main(int argc, char* argv[]) {
    if (argc < 5){
        printf("not enough args");
        exit(1);
    }

    char* dir_name = realpath(argv[1], NULL);
    if (!dir_name){
        printf("Cannot open dir");
        exit(1);
    }

    char* op = argv[2];

    time_t time = parse_time(argv[3]);

    char* mode = argv[4];

    if (strcmp(mode, "1") == 0) {
        tree(dir_name, op, time);
    } else if (strcmp(mode, "2") == 0) {
        gop = op;
        gtime = time;
        nftw(dir_name, tree_nftw, 20, FTW_PHYS);
    } else {
        printf("Wrong args");
    }

    free(dir_name);

    return 0;
}

