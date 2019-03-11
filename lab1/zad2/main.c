#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../zad1/library.h"

int main(int argc, char** argv) {

    if (argc < 2){
        printf("You have to give at least 1 argument.\n");
        printf("c size (create array of size)\n");
        printf("s dir file tmp (to search file in dir with tmp)\n");
        printf("r index (to remove block at given index number)\n");
        printf("ad number (to add and remove block number times)\n");
    }

    int size;
    struct search_wrapper* sw;

    int counter = 1;

    while (counter < argc){
        if (strcmp(argv[counter],"c") == 0){
            counter++;
            if (counter < argc){
                size = (int)strtol(argv[counter],NULL,10);
                delete_search_wrapper(sw);
                sw = init(size);
            } else{
                printf("Invalid arguments\n");
                exit(-1);
            }
            counter++;
        } else if (strcmp(argv[counter],"s") == 0){
            counter++;
            if (counter + 2 < argc){
                set_search_rules(sw, argv[counter],argv[counter+1],argv[counter+2]);
                search(sw);
                allocate_block(sw);
            } else{
                printf("Invalid arguments\n");
                exit(-1);
            }
            counter = counter + 3;
        } else if (strcmp(argv[counter],"r") == 0){
            counter++;
            if (counter < argc){
                int index = (int)strtol(argv[counter],NULL,10);
                remove_block(sw,index);
            } else{
                printf("Invalid arguments\n");
                exit(-1);
            }
            counter++;
        } else if (strcmp(argv[counter],"ad") == 0){
            counter++;
            if (counter < argc){
                int quantity = (int)strtol(argv[counter],NULL,10);
                for (int i = 0; i < quantity; ++i) {
                    int index = allocate_block(sw);
                    remove_block(sw,index);
                }
            } else{
                printf("Invalid arguments\n");
                exit(-1);
            }
            counter++;
        } else{
            printf("Invalid arguments\n");
            exit(-1);
        }
    }

}