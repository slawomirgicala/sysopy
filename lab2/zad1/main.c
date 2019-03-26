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
            fseek(file, min_pos, SEEK_SET);      //samo min_pos
            fread(buffer2, sizeof(char), record_length, file);
            fseek(file, i * record_length, SEEK_SET);
            fwrite(buffer2, sizeof(char), record_length, file);
            fseek(file, min_pos, SEEK_SET);
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
        read(file, &min, sizeof(char));
        for (int j = i+1; j < records_number; ++j) {
            lseek(file, j * record_length, SEEK_SET);
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



void copy_lib(const char* filepath1, const char* filepath2, int record_length, int records_number){
    FILE* copied = fopen(filepath1, "r");
    if (copied == NULL){
        perror("Cannot open file\n");
        exit(1);
    }
    FILE* copy = fopen(filepath2, "w");
    if (copy == NULL){
        perror("Cannot open file\n");
        exit(1);
    }

    char* buffer = calloc(1,record_length * sizeof(char));
    if (buffer == NULL){
        perror("Cannot allocate memory\n");
        exit(1);
    }

    for (int i = 0; i < records_number; i++) {
        if (fread(buffer, sizeof(char), record_length, copied) != record_length){
            perror("Cannot read record file\n");
            exit(1);
        }
        if (fwrite(buffer, sizeof(char), record_length, copy) != record_length){
            perror("Cannot write record file\n");
            exit(1);
        }
    }

    free(buffer);
    fclose(copied);
    fclose(copy);
}

void copy_sys(const char* filepath1, const char* filepath2, int record_length, int records_number){
    int copied = open(filepath1, O_RDONLY);
    if (copied == -1){
        perror("Cannot open file\n");
        exit(1);
    }
    int copy = open(filepath2, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (copy == -1){
        perror("Cannot open file\n");
        exit(1);
    }

    char* buffer = calloc(1,record_length * sizeof(char));
    if (buffer == NULL){
        perror("Cannot allocate memory\n");
        exit(1);
    }

    for (int i = 0; i < records_number; i++) {
        if (read(copied,buffer,record_length) != record_length){
            perror("Cannot read record file\n");
            exit(1);
        }
        if (write(copy,buffer,record_length) != record_length){
            perror("Cannot write record file\n");
            exit(1);
        }
    }

    free(buffer);
    close(copied);
    close(copy);
}

double calc_time(clock_t t1, clock_t t2) {
    return (double) (t2-t1) / sysconf(_SC_CLK_TCK);
}


int main(int argc, char** argv){

    srand(time(NULL));

    if (argc < 2){
        printf("Too little arguments");
        exit(1);
    }

    struct tms start;
    times(&start);

    if (strcmp(argv[1],"generate") == 0){
        if (argc != 5){
            printf("Wrong arguments");
            exit(1);
        }
        char* filepath = argv[2];
        int records_number = (int)strtol(argv[3],NULL,10);
        int record_length = (int)strtol(argv[4],NULL,10);
        generate(filepath,record_length,records_number);
    }else if (strcmp(argv[1],"sort") == 0){
        if (argc != 6){
            printf("Wrong arguments");
            exit(1);
        }
        char* filepath = argv[2];
        int records_number = (int)strtol(argv[3],NULL,10);
        int record_length = (int)strtol(argv[4],NULL,10);
        char* type = argv[5];
        if (strcmp(type,"sys") == 0){
            sort_sys(filepath,record_length,records_number);
        } else if (strcmp(type, "lib") == 0){
            sort_lib(filepath,record_length,records_number);
        } else{
            printf("Wrong type");
            exit(1);
        }

    }else if (strcmp(argv[1],"copy") == 0){
        if (argc != 7){
            printf("Wrong arguments");
            exit(1);
        }

        char* filepath1 = argv[2];
        char* filepath2 = argv[3];
        int records_number = (int)strtol(argv[4],NULL,10);
        int record_length = (int)strtol(argv[5],NULL,10);
        char* type = argv[6];
        if (strcmp(type,"sys") == 0){
            copy_sys(filepath1,filepath2,record_length,records_number);
        } else if (strcmp(type, "lib") == 0){
            copy_lib(filepath1,filepath2,record_length,records_number);
        } else{
            printf("Wrong type");
            exit(1);
        }

    }else{
        printf("Wrong arguments");
        exit(1);
    }

    struct tms end;
    times(&end);

    printf("User time: %0.2fs\n", calc_time(start.tms_utime, end.tms_utime));
    printf("System time: %0.2fs\n", calc_time(start.tms_stime, end.tms_stime));

    return 0;
}



