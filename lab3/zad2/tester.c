#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


int random_char(){
    return 'a' + (rand() % 26);
}


int main(int argc, char *argv[]) {
    srand(time(NULL));

    if (argc != 5){
        printf("Wrong args! Give: filename, pmin, pmax, bytes");
        exit(1);
    }

    char *file_name = argv[1];
    int pmin = (int) strtol(argv[2], NULL, 10);
    int pmax = (int) strtol(argv[3], NULL, 10);
    int bytes = (int) strtol(argv[4], NULL, 10);

    char* string = malloc(bytes+1);
    if (string == NULL){
        printf("Cannot allocate memory\n");
        exit(1);
    }

    char *date = malloc(20);


    while (1) {
        int wait = (rand() % (pmax - pmin + 1)) + pmin;
        sleep(wait);

        for (int i = 0; i < bytes; i++){
            string[i] = random_char();
        }
        string[bytes] = '\0';

        FILE *file = fopen(file_name, "a");

        time_t t = time(NULL);

        strftime(date, 20, "%d.%m.%Y %H:%M:%S", localtime(&t));

        fprintf(file, "%s %s %d\n", date, string, wait);

        fclose(file);
    }
}