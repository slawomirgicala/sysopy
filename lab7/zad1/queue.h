//
// Created by slawomir on 11.05.19.
//


#ifndef SYSOPY_QUEUE_H
#define SYSOPY_QUEUE_H


#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define SEMKEYPATH "/home"
#define SEMKEYID 21
#define SHMKEYPATH "/home"
#define SHMKEYID 37




double getCurrentTime() {
    struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec + currentTime.tv_usec *  1e-6;
}




typedef struct package{
    int weight;
    pid_t worker_pid;
    double pack_time;
}package;


typedef struct belt{
    int size;
    int front;
    int rear;
    int actual_packages_mass;
    int max_capacity;
    package queue[];
}belt;



int is_full(belt* b){
    if ((b->front = b->rear+1) || (b->front == 0 && b->rear == b->size-1)) return 1;
    return 0;
}

int is_empty(belt* b){
    if (b->front == -1) return 1;
    return 0;
}

int push(belt* b, package element){
    if (b->actual_packages_mass + element.weight > b->max_capacity){
        fprintf(stderr, "package is too heavy");
        return -1;
    }
    if (is_full(b)){
        fprintf(stderr, "belt is full");
        return -1;
    }
    if (b->front == -1) b->front = 0;
    b->rear = (b->rear+1)%b->size;
    b->queue[b->rear] = element;
    b->actual_packages_mass += element.weight;
    return 0;
}

package* pop(belt* b){
    if (is_empty(b)){
        return NULL;
    }
    package* result = &b->queue[b->front];
    if (b->front == b->rear){
        b->front = b->rear = -1;
    } else{
        b->front = (b->front+1)%b->size;
    }
    b->actual_packages_mass -= result->weight;
    return result;
}

int peek(belt* b){
    if (is_empty(b)){
        return -1;
    }
    return b->queue[b->front].weight;
}





void take_sem(int semid, unsigned short sem_num){
    struct sembuf buf;
    buf.sem_num = sem_num;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1){
        fprintf(stderr, "error with taking semaphore");
    }
}

void return_sem(int semid, unsigned short sem_num){
    struct sembuf buf;
    buf.sem_num = sem_num;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    if (semop(semid, &buf, 1) == -1){
        fprintf(stderr, "error with returning semaphore");
    }
}




#endif //SYSOPY_QUEUE_H
