//
// Created by slawomir on 12.05.19.
//


#define _XOPEN_SOURCE 500

#include "queue.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <signal.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>


int run = 1;
int max_package_weight;
int belt_size;



void handler(int sig) {
    run = 0;
}

void handle_signal(){
    struct sigaction act;
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
}

int get_weight(){
    return (int)(rand()%max_package_weight) + 1;
}


int main(int argc, char** argv){


    int number_of_cycles = -1;
    key_t semkey, shmkey;
    int semid, shmid;
    belt* shm_address;
    double creation_time;
    //struct shmid_ds shmid_struct;

    if (argc < 3){
        fprintf(stderr, "Wrong number of args, give: maximum package weight, belt size and number of cycles(optional)\n");
        return -1;
    }

    handle_signal();

    max_package_weight = atoi(argv[1]);
    belt_size = atoi(argv[2]);

    if (argc > 3){
        number_of_cycles = atoi(argv[3]);
    }


    semkey = ftok(SEMKEYPATH,SEMKEYID);
    if (semkey == (key_t)-1){
        fprintf(stderr,"main: ftok() for sem failed\n");
        return -1;
    }
    shmkey = ftok(SHMKEYPATH,SHMKEYID);
    if (shmkey == (key_t)-1){
        fprintf(stderr, "main: ftok() for shm failed\n");
        return -1;
    }

    semid = semget(semkey, 1, 0666);
    if (semid == -1){
        fprintf(stderr,"main: semget() failed, RUN TRUCKER FIRST\n");
        return -1;
    }

    size_t shm_seg = 4*sizeof(int) + belt_size*sizeof(package) + 100;

    shmid = shmget(shmkey, shm_seg, 0666);

    if (shmid == -1){
        fprintf(stderr, "main: shmget() failed, RUN TRUCKER FIRST\n");
        return -1;
    }

    /* Attach the shared memory segment to the server process.       */
    shm_address = (belt*)shmat(shmid, NULL, 0);
    if (shm_address == NULL){
        fprintf(stderr, "main: shmat() failed\n");
        return -1;
    }

    srand(time(NULL));

    printf("starting to work\n\n");

    while(run && number_of_cycles != 0){

        creation_time = get_current_time();
        //take_sem(semid,0);
        package new_package;
        new_package.weight = get_weight();
        new_package.worker_pid = getpid();
        new_package.pack_time = creation_time;
        while (1){
            take_sem(semid, 0);
            if (push(shm_address, new_package) == 0){
                return_sem(semid, 0);
                printf("Worker: %d - Time: %sPack was loaded, weight: %d\n\n",
                        new_package.worker_pid, get_ascii_time(), new_package.weight);
                break;
            }
            return_sem(semid, 0);
            printf("Worker: %d - Time: %sWaiting...\n\n", getpid(), get_ascii_time());
            usleep(100000);
        }
        if (number_of_cycles != -1) number_of_cycles--;
    }



    if (shmdt(shm_address) == -1){
        fprintf(stderr, "main: shmdt() failed\n");
    }


}

