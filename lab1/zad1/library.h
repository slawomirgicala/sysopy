#ifndef LIBRARY_LIBRARY_H
#define LIBRARY_LIBRARY_H

void hello(void);

typedef struct search_wrapper{
    int size;
    char** search_results;
    char* directory;
    char* file;
};

struct search_wrapper* init(int size);

void set_search_rules(struct search_wrapper sw, char* directory, char* file);

int search(struct search_wrapper sw);

void remove_block(struct search_wrapper sw, int index);


#endif