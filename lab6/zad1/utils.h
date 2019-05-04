//
// Created by slawomir on 04.05.19.
//

#ifndef SYSOPY_UTILS_H
#define SYSOPY_UTILS_H


const char PATH[] = "HOME";
const int PROJ_ID = 2137;



#define MSG_LEN_MAX 200
#define CLIENTS_MAX 10


typedef struct message{
    long type;
    pid_t sender;
    int client_id;
    char value[MSG_LEN_MAX];

}message;


const int MESSAGE_SIZE = sizeof(message) - sizeof(long);


// MSG TYPES

#define STOP 1
#define LIST 2
#define FRIENDS 3
#define ECHO 4
#define ADD 5
#define DEL 6
#define TOALL 7
#define TOFRIENDS 8
#define TOONE 9
#define INIT 10



key_t get_key() {
    key_t key = ftok(getenv(PATH), PROJ_ID);
    if (key == -1) {
        printf("error while creating key");
        exit(key);
    }
    return key;
}



#endif //SYSOPY_UTILS_H
