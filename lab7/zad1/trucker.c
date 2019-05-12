//
// Created by slawomir on 11.05.19.
//



#include "queue.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>





int truck_capacity;
int belt_capacity;
int belt_size;


//void set_sem_value(int semid, int semnum, int value) {
//    if (semctl(semid, semnum, SETVAL, value) == -1) {
//        fprintf(stderr, "Setting semaphore value");
//    }
//}



int main(int argc, char** argv){

    key_t semkey, shmkey;
    int semid, shmid;
    belt* shm_address;
    struct shmid_ds shmid_struct;


    if (argc != 4){
        fprintf(stderr, "Wrong number of args, give: truck capacity, belt capacity, belt size");
        return -1;
    }


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

    size_t shm_seg = 4*sizeof(int) + belt_capacity*sizeof(package) + 100;

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