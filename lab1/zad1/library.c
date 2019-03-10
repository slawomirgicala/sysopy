#include "library.h"

#include <stdio.h>

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

    //zmierz dlugosc pliku
    //zaalokuj pamiec
    //[rzepisz rzeczy z plku do zaalokowanej pamieci
}
