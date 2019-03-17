#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/times.h>



int random_char(){
    return 'a' + (rand() % 26);
}

void generate(const char* filepath, int record_length, int records_number){

    FILE* file = fopen(filepath, "w");
    if (file == NULL){
        perror("Cannot open file\n");
        exit(1);
    }
    for (int i = 0; i < records_number; ++i) {
        for (int j = 0; j < record_length; ++j) {
            fputc(random_char(), file);
        }
    }
    fclose(file);
}


void sort_lib(const char* filepath, int record_length, int records_number){
    FILE* file = fopen(filepath, "r+");
    if (file == NULL){
        perror("Cannot open file\n");
        exit(1);
    }

    unsigned char min;
    unsigned char tmp;
    int min_pos;

    char* buffer1 = calloc(1, record_length * sizeof(char));
    char* buffer2 = calloc(1, record_length * sizeof(char));

    if (buffer1 == NULL || buffer2 == NULL){
        perror("Cannot allocate memory for buffers\n");
        exit(1);
    }

    for (int i = 0; i < records_number - 1; ++i) {
        fseek(file, i * record_length, SEEK_SET);
        min_pos = i * record_length;
        min = (unsigned char) fgetc(file);
        for (int j = i+1; j < records_number; ++j) {
            fseek(file, j * record_length, SEEK_SET);
            tmp = (unsigned char) fgetc(file);
            if (tmp < min){
                min = tmp;
                min_pos = j * record_length;
            }
        }
        if (i * record_length != min_pos){
            fseek(file, i * record_length, SEEK_SET);
            fread(buffer1, sizeof(char), record_length, file);
            fseek(file, min_pos * record_length, SEEK_SET);
            fread(buffer2, sizeof(char), record_length, file);
            fseek(file, i * record_length, SEEK_SET);
            fwrite(buffer2, sizeof(char), record_length, file);
            fseek(file, min_pos * record_length, SEEK_SET);
            fwrite(buffer1, sizeof(char), record_length, file);
        }
    }
    free(buffer1);
    free(buffer2);
    fclose(file);
}



void sort_sys(const char* filepath, int record_length, int records_number){
    int file = open(filepath, O_RDWR);
    if (file == -1){
        perror("Cannot open file\n");
        exit(1);
    }

    unsigned char min;
    unsigned char tmp;
    int min_pos;

    char* buffer1 = calloc(1, record_length * sizeof(char));
    char* buffer2 = calloc(1, record_length * sizeof(char));

    if (buffer1 == NULL || buffer2 == NULL){
        perror("Cannot allocate memory for buffers\n");
        exit(1);
    }

    for (int i = 0; i < records_number - 1; ++i) {
        lseek(file, i * record_length, SEEK_SET);
        min_pos = i * record_length;
        //min = (unsigned char) fgetc(file);
        read(file, &min, sizeof(char));
        for (int j = i+1; j < records_number; ++j) {
            lseek(file, j * record_length, SEEK_SET);
            //tmp = (unsigned char) fgetc(file);
            read(file, &tmp, sizeof(char));
            if (tmp < min){
                min = tmp;
                min_pos = j * record_length;
            }
        }
        if (i * record_length != min_pos){
            lseek(file, i * record_length, SEEK_SET);
            read(file, buffer1, record_length * sizeof(char));
            lseek(file, min_pos * record_length, SEEK_SET);
            read(file, buffer2, record_length * sizeof(char));
            lseek(file, i * record_length, SEEK_SET);
            write(file, buffer2, record_length * sizeof(char));
            lseek(file, min_pos * record_length, SEEK_SET);
            write(file, buffer1, record_length * sizeof(char));
        }
    }
    free(buffer1);
    free(buffer2);
    close(file);
}





int main(int argc, char** argv){

    srand(time(NULL));
    generate("wyniki.txt", 1, 100);
    sort_lib("wyniki.txt", 1, 100);
    return 0;
}



