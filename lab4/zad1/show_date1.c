#define _XOPEN_SOURCE 500
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int show_date = 1;

void sig_tstp(int);
void sig_int(int);

void die(char*);

int main(void) {

    struct sigaction sig_tstp_action;
    sig_tstp_action.sa_handler = sig_tstp;
    sigemptyset(&sig_tstp_action.sa_mask);
    sig_tstp_action.sa_flags = 0;

    if (sigaction(SIGTSTP, &sig_tstp_action, NULL) != 0)
        die("can't catch SIGTSTP");

    if (signal(SIGINT, sig_int) == SIG_ERR)
        die("can't catch SIGINT");

    while (1) {
        if (show_date) {
            time_t now;
            time(&now);
            printf("%s", ctime(&now));
        }
        sleep(1);
    }
    return 0;
}

void sig_tstp(int x) {
    if (show_date) {
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTR+C - zakończenie programu");
        show_date = 0;
    } else {
        show_date = 1;
    }
}

void sig_int(int x) {
    printf("Odebrano sygnał SIGINT");

    if (signal(SIGINT, sig_int) == SIG_ERR)
        die("can't catch SIGINT");
    exit(1);
}

void die(char* msg) {
    fprintf(stderr, "%s\n", msg);
    exit(1);
}