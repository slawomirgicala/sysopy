#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "utils.h"




int run = 1;
int server_queue;
int client_queue;
int my_id;



void send_message(message msg, int queue) {
    if (msgsnd(queue, &msg, MESSAGE_SIZE, 0) == -1) {
        fprintf(stderr, "error while sending a message %s of type %ld\n", msg.value, msg.type);
        perror(strerror(errno));
        msgctl(client_queue, IPC_RMID, NULL);
        exit(-1);
    }
}

int init(int queue_key){
    //char private_q_str[25];

    message msg;
    sprintf(msg.value, "%d", queue_key);
    //msg.sender = getpid();
    msg.type = INIT;
    send_message(msg, server_queue);

    sleep(1);
    msgrcv(client_queue, &msg, (size_t) MESSAGE_SIZE, 0, MSG_NOERROR);
    if (strcmp(msg.value, "server is full") == 0) {
        printf("failure, server is full");
        return -1;
    } else {
        printf("%s", msg.value);
        my_id = atoi(msg.value);
    }
    return 0;
}

char *str_cut(char *str, int begin, int len) {
    int l = strlen(str);

    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);

    return str;
}


long get_type(char type[]) {

    if(strcmp("ECHO", type) == 0){
        return ECHO;
    }else if (strcmp("LIST", type) == 0){
        return LIST;
    }else if (strcmp("FRND", type) == 0){
        return FRIENDS;
    }else if (strcmp("ADDF", type) == 0){
        return ADD;
    }else if (strcmp("DELF", type) == 0){
        return DEL;
    }else if (strcmp("2ALL", type) == 0){
        return TOALL;
    }else if (strcmp("2FRI", type) == 0){
        return TOFRIENDS;
    }else if (strcmp("2ONE", type) == 0){
        return TOONE;
    }else if (strcmp("STOP", type) == 0){
        return STOP;
    }

    return -1;

}


void handler(int sign) {
    run = 0;
    message m;
    m.type = STOP;
    m.sender = my_id;
    send_message(m,server_queue);
    msgctl(client_queue, IPC_RMID, NULL);
}

void handle_signal(){
    struct sigaction act;
    act.sa_handler = &handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);
}

int generate_number(){
    pid_t pid = getpid();
    return 123*pid + 1000;
}

int main(){

    handle_signal();

    key_t server_key = get_key();
    server_queue = msgget(server_key,0);

    if (server_queue == -1){
        fprintf(stderr, "Cannot open server queue");
        exit(-1);
    }

    int client_key = generate_number();
    client_queue = msgget(client_key, 0666 | IPC_CREAT);
    if (client_queue == -1){
        fprintf(stderr, "cannot open private queue");
        perror(strerror(errno));
        exit(-1);
    }

    if(init(client_key) == -1){
        exit(-1);
    }


    printf("Connection successful\n"
           "use \n"
           "ECHO string\t\tfor echo\n"
           "LIST\t\t\tfor listing all clients\n"
           "FRND friends_list\tto make friends list\n"
           "ADDF friends_list\tto add to friends list\n"
           "DELF friends_list\tto delete from friends list\n"
           "2ALL string\t\tto send message to all clients\n"
           "2FRI string\t\tto send message to friends\n"
           "2ONE id string\t\tto send message to user of id\n"
           "STOP\t\t\tto stop client\n"
    );



    char *buffer;
    size_t bufsize = 32;
    buffer = (char *)malloc(bufsize * sizeof(char));
    message msg;
    char * b;
    long type;




    while (run){
        printf("\n%d@serv: >\t ", my_id);
        getline(&buffer,&bufsize,stdin);
        char type_str[MSG_LEN_MAX];
        strncpy(type_str, buffer, 4);
        type_str[4] = 0;
        type = get_type(type_str);
        if (type == -1){
            printf("Wrong command!\n");
            continue;
        }

        b = str_cut(buffer,0,5);
        strcpy(msg.value, b); //zrobic cos jak add i del bez arg


        msg.sender = my_id;
        msg.type = type;
        send_message(msg, server_queue);
        if (type == STOP) {
            msgctl(client_queue, IPC_RMID, NULL);
            exit(0);
        }
        msgrcv(client_queue, &msg, (size_t) MESSAGE_SIZE, 0, MSG_NOERROR);
        if (msg.type == STOP){
            msgctl(client_queue, IPC_RMID, NULL);
            exit(0);
        }
        printf("%s", msg.value);
    }


}