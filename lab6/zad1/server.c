//
// Created by slawomir on 04.05.19.
//

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include "utils.h"
#include <signal.h>

int run = 1;

typedef struct client{
    int id;
    int queue_id;
    //pid_t pid;
    int friends[CLIENTS_MAX];
} client;

client* clients[CLIENTS_MAX];
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

int add_client(pid_t client_pid, int queue_key){
    int i;
    for (i = 0; i < CLIENTS_MAX; i++){
        if (clients[i] == NULL) break;
    }
    if (i == CLIENTS_MAX){
        fprintf(stderr, "too many clients, cannot connect!\n");
        return -1;
    }
    int queue_id;
    if ((queue_id = msgget(queue_key,0 /*IPC_EXCL | IPC_CREAT | 0666*/)) == -1){
        if (errno != EEXIST){
            fprintf(stderr, "wrong client queue id (key)\n");
            return -2;
        } else{
            perror(strerror(errno));
        }
    }
    client* new_client = malloc(sizeof(client));
    new_client->id = current_id;
    //new_client.pid = client_pid;
    new_client->queue_id = queue_id;
    for (int j = 0; j < CLIENTS_MAX; ++j) {
        new_client->friends[j] = -1;
    }

    clients[i] = new_client;

    current_id++;
    return current_id-1;
}


void send_message(char* value, int queue_id, long type) {
    message msg;
    strcpy(msg.value, value);
    msg.type = type;
    //msg.sender = -1;//SERVER

    if (msgsnd(queue_id, &msg, MESSAGE_SIZE, 0) == -1) {
        fprintf(stderr, "error while sending a message %s of type %ld to queue %d\n", value, type, queue_id);
        perror(strerror(errno));
        //printf(errno);
    }
}

int get_client_queue(int id){
    for (int i = 0; i < CLIENTS_MAX; ++i) {
        if (clients[i] != NULL){
            if (clients[i]->id == id){
                return clients[i]->queue_id;
            }
        }
    }
    fprintf(stderr, "no queue of such client's id");
    return -1;
}

/*int get_client_id(pid_t pid){
    int id = 0;
    for (int i = 0; i < CLIENTS_MAX; ++i) {
        if (clients[i] != NULL){
            if (clients[i]->pid == pid){
                id = clients[i]->id;
                break;
            }
        }
    }
    return id;
}*/

int get_client_position(int id){
    int pos = 0;
    for (int i = 0; i < CLIENTS_MAX; ++i) {
        if (clients[i] != NULL){
            if (clients[i]->id == id){
                pos = i;
                break;
            }
        }
    }
    return pos;
}



void process(message* msg){
    if (msg != NULL){
        if (msg->type == INIT){
            printf("login attempt from %d\n", msg->sender);
            int is_added_id = add_client(msg->sender,atoi(msg->value));
            if (is_added_id == -1){
                send_message("server is full", atoi(msg->value), INIT);   // tu powinno sie wysylac w msg queueid
            }else if (is_added_id > 0){
                char id[20];
                sprintf(id, "%d", is_added_id);
                int pos = get_client_position(is_added_id);
                printf("\n%d\n", clients[pos]->queue_id);
                send_message(id,clients[pos]->queue_id /*atoi(msg->value)*/, INIT);
            }
        }else if (msg->type == LIST){

            char clients_list[MSG_LEN_MAX];
            strcpy(clients_list, "Clients:\n");
            for (int i = 0; i < CLIENTS_MAX; ++i) {
                if (clients[i] != NULL){
                    char client_id[20];
                    sprintf(client_id, "%d", clients[i]->id);
                    strcat(clients_list, client_id);
                    strcat(clients_list, "\n");
                }
            }
            int queue = get_client_queue(msg->sender);
            if (queue != -1){
                send_message(clients_list, queue, LIST);
            }
        } else if (msg->type == FRIENDS){

            for (int i = 0; i < CLIENTS_MAX; ++i) {
                if (clients[i] != NULL){
                    if (clients[i]->id == msg->sender){
                        for (int j = 0; j < CLIENTS_MAX; ++j) {
                            clients[i]->friends[j] = -1;
                        }
                        int k = 0;
                        char *token = strtok(msg->value, " ");
                        while (token != NULL)
                        {
                            int friend = atoi(token);
                            clients[i]->friends[k] = friend;
                            k++;
                            token = strtok(NULL, " ");
                        }
                        break;
                    }
                }
            }
        }else if (msg->type == ADD){
            for (int i = 0; i < CLIENTS_MAX; ++i) {
                if (clients[i] != NULL){
                    if (clients[i]->id == msg->sender){
                        char *token = strtok(msg->value, " ");
                        while (token != NULL)
                        {
                            int friend = atoi(token);
                            //clients[i]->friends[k] = friend;
                            int present = 0;
                            for (int j = 0; j < CLIENTS_MAX; ++j) {
                                if(clients[i]->friends[j] == friend){
                                    present = 1;
                                    break;
                                }
                            }
                            if (!present){
                                for (int k = 0; k < CLIENTS_MAX; ++k) {
                                    if(clients[i]->friends[k] == -1){
                                        clients[i]->friends[k] = friend;
                                        break;
                                    }
                                }
                            }
                            token = strtok(NULL, " ");
                        }
                        break;
                    }
                }
            }
        }else if (msg->type == DEL){
            for (int i = 0; i < CLIENTS_MAX; ++i) {
                if (clients[i] != NULL){
                    if (clients[i]->id == msg->sender){
                        char *token = strtok(msg->value, " ");
                        while (token != NULL)
                        {
                            int friend = atoi(token);
                            //clients[i]->friends[k] = friend;
                            //int present = 0;
                            for (int j = 0; j < CLIENTS_MAX; ++j) {
                                if(clients[i]->friends[j] == friend){
                                    clients[i]->friends[j] = -1;
                                    break;
                                }
                            }
                            token = strtok(NULL, " ");
                        }
                        break;
                    }
                }
            }
        }else if (msg->type == ECHO){
            int queue = clients[get_client_position(msg->sender)]->queue_id;  //              ID
            if (queue != -1){
                printf("\n%d\n", queue);
                send_message(msg->value, queue, ECHO);
            }
        }else if (msg->type == TOALL){
            int id = msg->sender;
            int queue = get_client_queue(msg->sender);
            char mes[MSG_LEN_MAX];

            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(mes, sizeof(mes)-1, "%d %m %Y %H:%M", t);

            strcat(mes, "FROM ");
            char id_str[12];
            sprintf(id_str, "%d", id);

            strcat(mes, id_str);
            strcat(mes, ": ");
            strcat(mes, msg->value);

            if (queue != -1){
                for (int i = 0; i < CLIENTS_MAX; ++i) {
                    if (clients[i] != NULL){
                        printf("\n%d\n", clients[i]->queue_id);

                        send_message(mes, clients[i]->queue_id, TOALL);
                    }
                }
            }
        }else if (msg->type == TOFRIENDS){
            int id = msg->sender;
            int queue = get_client_queue(msg->sender);
            char message[MSG_LEN_MAX];

            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(message, sizeof(message)-1, "%d %m %Y %H:%M", t);

            strcat(message, "FROM ");
            char id_str[12];
            sprintf(id_str, "%d", id);

            strcat(message, id_str);
            strcat(message, ": ");
            strcat(message, msg->value);

            if (queue != -1){
                for (int i = 0; i < CLIENTS_MAX; ++i) {
                    if (clients[i] != NULL){
                        if (clients[i]->id == id){
                            for (int j = 0; j < CLIENTS_MAX; ++j) {
                                if (clients[i]->friends[j] != -1){
                                    send_message(message, get_client_queue(clients[i]->friends[j]), TOFRIENDS);
                                }
                            }
                            break;
                        }
                    }
                }
            }
        }else if (msg->type == TOONE){

            int id = msg->sender;
            //int queue = get_client_queue(msg->sender);
            char message[MSG_LEN_MAX];

            time_t now = time(NULL);
            struct tm *t = localtime(&now);
            strftime(message, sizeof(message)-1, "%d %m %Y %H:%M", t);

            strcat(message, "FROM ");
            char id_str[12];
            sprintf(id_str, "%d", id);

            strcat(message, id_str);
            strcat(message, ": ");
            //strcat(message, msg->value);

            int receiver;
            char real_message[MSG_LEN_MAX];

            sscanf(msg->value, "%d %s", &receiver, real_message);
            strcat(message, real_message);

            for (int i = 0; i < CLIENTS_MAX; ++i) {
                if (clients[i] != NULL){
                    if (clients[i]->id == receiver){
                        send_message(message, clients[i]->queue_id, TOONE);
                    }
                }
            }

        }else if (msg->type == STOP){
            for (int i = 0; i < CLIENTS_MAX; ++i) {
                if (clients[i] != NULL){
                    if (clients[i]->id == msg->sender){
                        free(clients[i]);
                        clients[i] = NULL;
                    }
                }
            }
        }else{
            fprintf(stderr, "Wrong message type");
        }

    }
}


void close_queue(int qid){
    for (int i = 0; i < CLIENTS_MAX; ++i) {
        if (clients[i] != NULL){
            send_message("",clients[i]->queue_id,STOP);
            free(clients[i]);
        }
    }
    msgctl(qid, IPC_RMID, NULL);
}


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


int main(){

    for (int i = 0; i < CLIENTS_MAX; ++i) {
        clients[i] = NULL;
    }

    handle_signal();

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


    close_queue(queue);


    return 0;
}






