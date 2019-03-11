#ifndef LIBRARY_LIBRARY_H
#define LIBRARY_LIBRARY_H


typedef struct search_wrapper{
    int size;
    char** search_results;
    char* directory;
    char* file;
    char* tmp_file;
};

struct search_wrapper* init(int size);

void set_search_rules(struct search_wrapper* sw, char* directory, char* file, char* tmp_file);

int search(struct search_wrapper* sw);

void remove_block(struct search_wrapper* sw, int index);

void delete_search_wrapper(struct search_wrapper* sw);

#endif