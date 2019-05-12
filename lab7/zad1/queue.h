//
// Created by slawomir on 11.05.19.
//


#ifndef SYSOPY_QUEUE_H
#define SYSOPY_QUEUE_H


#include <stdio.h>
#include <errno.h>


#define SEMKEYPATH "/home"
#define SEMKEYID 21
#define SHMKEYPATH "/home"
#define SHMKEYID 37





typedef struct package{
    int weight;
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






#endif //SYSOPY_QUEUE_H
