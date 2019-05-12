//
// Created by slawomir on 12.05.19.
//




#include "queue.h"

#include <sys/types.h>
#include <sys/wait.h>




int main(int argc, char** argv){

    if (argc < 4){
        fprintf(stderr, "Wrong args! Give: number of workers, belt size, max package mass, (optional number of cycles)\n");
        return -1;
    }

    int n = atoi(argv[1]);

    if (argc > 4){
        for (int i = 0; i < n; ++i) {
            if (fork() == 0){
                execl("./worker", "worker", argv[3], argv[2], argv[4], NULL);
            }
        }

    }else{
        for (int i = 0; i < n; ++i) {
            if (fork() == 0){
                execl("./worker", "worker", argv[3], argv[2], NULL);
            }
        }

    }

    pid_t wpid;
    int status;
    while ((wpid = wait(&status)) > 0);
}