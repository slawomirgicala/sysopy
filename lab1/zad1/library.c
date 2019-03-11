#include "library.h"

#include <stdio.h>
#include <stdlib.h>

struct search_wrapper* init(int size){

    if (size <= 0){
        fprintf(stderr,"Array size has to be bigger than 0");
        exit(-1);
    }

    struct search_wrapper* sw = calloc(1, sizeof(struct search_wrapper));
    char** results = calloc(size, sizeof(char*));

    if (sw == NULL || array == NULL){
        fprintf(stderr, "Could not allocate memory");
        exit(-1);
    }

    sw->size = size;
    sw->search_results = results;

    return sw;
}

void set_search_rules(struct search_wrapper* sw, char* directory, char* file, char* tmp_file){

    sw->directory = directory;
    sw->file = file;
    sw->tmp_file = tmp_file;

}

int search(struct search_wrapper* sw){

    if (sw == NULL || sw->search_results == NULL || sw->directory == NULL || sw->file == NULL || sw->tmp_file == NULL){
        fprintf(stderr, "Correct data is not specified");
        exit(-1);
    }

    char* command = calloc(1,strlen(bt->dir) + strlen(bt->file) + strlen(bt->temp) + 25);
    sprintf(command, "find \"%s\" -iname \"%s\" > \"%s\"", sw->directory, sw->file, sw->tmp_file);

    system(command);

    free(command);

    int i = 0;
    while (i < sw->size && sw->search_results[i] != NULL){
        i++;
    }

    if (sw->search_results[i] != NULL){
        fprintf(stderr,"Array is full");
        exit(-1);
    }

    FILE* buffer = fopen(sw->tmp_file, "r");
    if (buffer == NULL){
        fprintf(stderr, "There is no such temporary file");
        exit(-1);
    }

    fseek(buffer, 0, 2);
    long buffer_size = ftell(buffer);
    rewind(buffer);

    sw->search_results[i] = calloc(1, buffer_size + 1);
    if (sw->search_results[i] == NULL){
        fprintf(stderr, "Could not allocate block");
        fclose(buffer);
        exit(-1);
    }

    fread(sw->search_results[i],buffer_size,1,buffer);

    fclose(buffer);

    return i;
}

void remove_block(struct search_wrapper* sw, int index){
    if (sw == NULL || sw->search_results == NULL){
        fprintf(stderr, "Block does not exist");
        exit(-1);
    }
    if (index < 0 || index >= sw->size){
        fprintf(stderr, "Index is out of results array boundries");
        exit(-1);
    }
    if (sw->search_results[index] != NULL){
        free(sw->search_results[index]);
        sw->search_results[index] = NULL;
    }
}

void delete_search_wrapper(struct search_wrapper* sw) {
    if (sw != NULL) {
        if (sw->search_results != NULL) {
            for (int i = 0; i < sw->size; i++) {
                if (sw->search_results[i] != NULL)
                    free(sw->search_results[i]);
            }
            free(sw->search_results);
        }
        free(sw);
    }
}




























