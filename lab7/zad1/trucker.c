//
// Created by slawomir on 11.05.19.
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





int truck_capacity;
int belt_capacity;
int belt_size;
int actual_truck_weight;

int run = 1;

//void set_sem_value(int semid, int semnum, int value) {
//    if (semctl(semid, semnum, SETVAL, value) == -1) {
//        fprintf(stderr, "Setting semaphore value");
//    }
//}


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





int main(int argc, char** argv){

    key_t semkey, shmkey;
    int semid, shmid;
    belt* shm_address;
    struct shmid_ds shmid_struct;


    if (argc != 4){
        fprintf(stderr, "Wrong number of args, give: truck capacity, belt capacity, belt size");
        return -1;
    }

    handle_signal();

    truck_capacity = atoi(argv[1]);
    belt_capacity = atoi(argv[2]);
    belt_size = atoi(argv[3]);

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

    semid = semget(semkey, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if (semid == -1){
        fprintf(stderr,"main: semget() failed\n");
        return -1;
    }

    if (semctl(semid, 0, SETVAL, 1) == -1) {
        fprintf(stderr, "Setting semaphore value, initialization");
    }

    size_t shm_seg = 4*sizeof(int) + belt_size*sizeof(package) + 100;

    shmid = shmget(shmkey, shm_seg, 0666 | IPC_CREAT | IPC_EXCL);

    if (shmid == -1){
        fprintf(stderr, "main: shmget() failed\n");
        return -1;
    }

    /* Attach the shared memory segment to the server process.       */
    shm_address = (belt*)shmat(shmid, NULL, 0);
    if (shm_address == NULL){
        fprintf(stderr, "main: shmat() failed\n");
        return -1;
    }

    shm_address->front = shm_address->rear = -1;
    shm_address->max_capacity = belt_capacity;
    shm_address->actual_packages_mass = 0;
    shm_address->size = belt_size;


    /*package p;
    p.weight = 5;
    push(shm_address,p);

    printf("queue has package: %d", peek(shm_address));*/ //NICE!


    printf("%s", "Empty truck arrives\n\n");
    actual_truck_weight = 0;
    int next_package_weight;
    while(run){
        take_sem(semid, 0);
        next_package_weight = peek(shm_address);
        if (next_package_weight != -1){
            if (actual_truck_weight + next_package_weight > truck_capacity){
                printf("Truck is full\nTruck leaves\nNew empty truck arrives\n\n");
                actual_truck_weight = 0;
            } else{
                package* next = pop(shm_address);
                printf("Packing package into the truck:\nFrom: %d\nJourney time: %f\nWeight: %d\nTruck free place: %d\nActual mass: %d\n\n",
                        next->worker_pid, get_current_time()-next->pack_time, next->weight, truck_capacity-actual_truck_weight, actual_truck_weight);
                actual_truck_weight += next_package_weight;
            }
        } else{
            printf("No package on the belt, waiting...\n\n");
        }
        return_sem(semid, 0);
        usleep(1000000);
    }

    take_sem(semid, 0);

    while ((next_package_weight = peek(shm_address)) != -1){
        if (actual_truck_weight + next_package_weight > truck_capacity){
            printf("Truck is full\nTruck leaves\nNew empty truck arrives\n\n");
            actual_truck_weight = 0;
        } else{
            package* next = pop(shm_address);
            printf("Packing package into the truck:\nFrom: %d\nJourney time: %f\nWeight: %d\nTruck free place: %d\nActual mass: %d\n\n",
                   next->worker_pid, get_current_time()-next->pack_time, next->weight, truck_capacity-actual_truck_weight, actual_truck_weight);
            actual_truck_weight += next_package_weight;
        }
    }



    //printf("%s", "after infnite loop");




    if(semctl(semid,0,IPC_RMID) == -1){
        fprintf(stderr,"remove failed");
    }

    if(shmdt(shm_address) == -1){
        fprintf(stderr,"remove failed");
    }

    if(shmctl(shmid,IPC_RMID,&shmid_struct) == -1){
        fprintf(stderr,"remove failed");
    }

}