#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include "library.h"



typedef struct time_keeper{
    double real_time;
    double user_time;
    double system_time;
}time_keeper;


time_keeper* prepare_time_keeper(){
    time_keeper* result = calloc(1, sizeof(time_keeper));
    result->real_time = 0;
    result->system_time = 0;
    result->user_time = 0;
    return result;
}

double calculate_time(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}


void add_time(time_keeper* tk, struct tms start, struct tms end, clock_t rel_start, clock_t rel_end){
    tk->real_time += calculate_time(rel_start,rel_end);
    tk->user_time += calculate_time(start.tms_utime, end.tms_utime);
    tk->system_time += calculate_time(start.tms_stime,end.tms_stime);
}

void print_time_keeper(time_keeper* tk, const char* type){
    printf("\n%s\n",type);
    printf("Real time: %.5f\n", tk->real_time);
    printf("User time: %.5f\n", tk->user_time);
    printf("System time: %.5f\n", tk->system_time);
    FILE* raport = fopen("./raport3a.txt","a");
    if (raport == NULL){
        printf("%s", "Cannot make raport");
        exit(-1);
    }
    fprintf(raport,"\n%s\n",type);
    fprintf(raport,"Real time: %.5f\n", tk->real_time);
    fprintf(raport,"User time: %.5f\n", tk->user_time);
    fprintf(raport,"System time: %.5f\n", tk->system_time);
    fclose(raport);
}


int main(int argc, char** argv) {

    if (argc < 2){
        printf("You have to give at least 1 argument.\n");
        printf("c size (create array of size)\n");
        printf("s dir file tmp (to search file in dir with tmp)\n");
        printf("r index (to remove block at given index number)\n");
        printf("ad number (to add and remove block number times)\n");
        exit(-1);
    }

    time_keeper* searching_time = prepare_time_keeper();
    time_keeper* allocating_time = prepare_time_keeper();
    time_keeper* removing_time = prepare_time_keeper();
    time_keeper* add_and_rem_time = prepare_time_keeper();


    int size;
    struct search_wrapper* sw = init(1);

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

                struct tms start1;
                clock_t rel_start1 = times(&start1);

                search(sw);

                struct tms end1;
                clock_t rel_end1 = times(&end1);
                add_time(searching_time,start1,end1,rel_start1,rel_end1);

                struct tms start2;
                clock_t rel_start2 = times(&start2);

                allocate_block(sw);

                struct tms end2;
                clock_t rel_end2 = times(&end2);
                add_time(allocating_time,start2,end2,rel_start2,rel_end2);

            } else{
                printf("Invalid arguments\n");
                exit(-1);
            }
            counter = counter + 3;
        } else if (strcmp(argv[counter],"r") == 0){
            counter++;
            if (counter < argc){
                int index = (int)strtol(argv[counter],NULL,10);

                struct tms start;
                clock_t rel_start = times(&start);

                remove_block(sw,index);

                struct tms end;
                clock_t rel_end = times(&end);
                add_time(removing_time,start,end,rel_start,rel_end);

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

                    struct tms start;
                    clock_t rel_start = times(&start);

                    int index = allocate_block(sw);
                    remove_block(sw,index);

                    struct tms end;
                    clock_t rel_end = times(&end);
                    add_time(add_and_rem_time,start,end,rel_start,rel_end);

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

    print_time_keeper(searching_time, "Searching time");
    print_time_keeper(allocating_time, "Allocating time");
    print_time_keeper(removing_time, "Removing time");
    print_time_keeper(add_and_rem_time, "Adding and removing time");

    delete_search_wrapper(sw);
    free(searching_time);
    free(allocating_time);
    free(add_and_rem_time);
    free(removing_time);
    return 0;
}