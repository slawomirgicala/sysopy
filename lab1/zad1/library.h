#ifndef LIBRARY_LIBRARY_H
#define LIBRARY_LIBRARY_H


struct search_wrapper{
    int size;
    char** search_results;
    char* directory;
    char* file;
    char* tmp_file;
};

struct search_wrapper* init(int size);

int allocate_block(struct search_wrapper* sw);

void set_search_rules(struct search_wrapper* sw, char* directory, char* file, char* tmp_file);

void search(struct search_wrapper* sw);

void remove_block(struct search_wrapper* sw, int index);

void delete_search_wrapper(struct search_wrapper* sw);

#endif