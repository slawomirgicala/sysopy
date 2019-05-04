//
// Created by slawomir on 04.05.19.
//


#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "utils.h"

int run = 1;

typedef struct client{
    int id;
    int queue_id;
    pid_t pid;
    int friends[CLIENTS_MAX];
} client;

client clients[CLIENTS_MAX];
int current_id = 1;



message* get_message(int queue, message *msg, int MSG_SIZE) {
    size_t size = (size_t) MESSAGE_SIZE;

    if (msgrcv(queue, msg, size, -4, MSG_NOERROR | IPC_NOWAIT) != -1) {
        return msg;
    }else if (msgrcv(queue, msg, size, 0, MSG_NOERROR | IPC_NOWAIT) != -1){
        return msg;
    }
    return NULL;
}

int add_client(pid_t client_pid, int queue_id){
    int i;
    for (i = 0; i < CLIENTS_MAX; i++){
        if (clients[i] == NULL) break;
    }
    if (i == CLIENTS_MAX){
        fprintf(stderr, "too many clients, cannot connect!\n");
        return -1;
    }
    if (msgget(queue_id, IPC_EXCL | IPC_CREAT | 0666)){
        if (errno != EEXIST){
            fprintf(stderr, "wrong client queue id (key)\n");
            return -2;
        }
    }
    client new_client;
    new_client.id = current_id;
    new_client.pid = client_pid;
    new_client.queue_id = queue_id;
    for (int j = 0; j < CLIENTS_MAX; ++j) {
        new_client.friends[j] = -1;
    }
    current_id++;
    return current_id-1;
}


void send_message(char* value, int queue_id, long type) {
    Message msg;
    strcpy(msg.value, value);
    msg.type = type;
    msg.sender = -1;//SERVER

    if (msgsnd(queue_id, &msg, MESSAGE_SIZE, 0) == -1) {
        fprintf(stderr, "error while sending a message %s of type %l to queue %d\n", value, type, queue_id);
    }
}

int get_client_queue(int id){
    for (int i = 0; i < CLIENTS_MAX; ++i) {
        if (clients[i] != NULL){
            if (clients[i].id == id){
                return clients[i].queue_id;
            }
        }
    }
    fprintf(stderr, "no queue of such client's id");
    return -1;
}

void process(message* msg){
    if (msg != NULL){
        if (msg->type == INIT){
            printf("login attempt from %d\n", msg->sender);
            int is_added = add_client(msg->sender,atoi(msg->value));
            if (is_added == -1){
                send_message("server is full", atoi(msg->value), INIT);
            }else if (is_added > 0){
                char id[20];
                sprintf(id, "%d", is_added);
                send_message(id, atoi(msg->value), INIT);
            }
        }else if (msg->type == LIST){

            char clients_list[MSG_LEN_MAX];
            strcpy(clients_list, "Clients:\n");
            for (int i = 0; i < CLIENTS_MAX; ++i) {
                if (clients[i] != NULL){
                    strcat(clients_list, atoi(clients[i].id));
                    strcat(clients_list, "\n");
                }
            }
            int queue = get_client_queue(msg->client_id);
            if (queue != -1){
                send_message(clients_list, queue, LIST);
            }
        } else if (msg->type == FRIENDS){
            
        }

    }
}



int main(){
    for (int i = 0; i < CLIENTS_MAX; ++i) {
        clients[i] = NULL;
    }

    key_t key = get_key();
    int flags = IPC_CREAT | 0666;
    int queue = msgget(key, flags);
    if (queue == -1){
        fprintf(stderr, "error while creating server queue.\n");
        exit(-1);
    }

    message msg;

    while(run){

        //msgrcv(queue,&msg,size,-4,flagggg);
        process(get_message(queue, &msg, MESSAGE_SIZE));


    }


    close_queue();


}






