// sleepy.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <seconds>\n", argv[0]);
        exit(1);
    }

    int seconds = atoi(argv[1]);
    for (int i = 0; i < seconds; i++) {
        printf("PID %d: Sleeping... (%d/%d)\n", getpid(), i + 1, seconds);
        fflush(stdout);
        sleep(1);
    }

    printf("PID %d: Done.\n", getpid());
    fflush(stdout);
    return 0;
}

