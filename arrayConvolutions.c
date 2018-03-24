/*
 * A concurrent program that perform arithmetic operations on an array of
 * a given size whilst maintaining atomicity on each element of the array.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define THREAD_CREATION_FAILED -1
#define THREAD_JOIN_FAILED -2

typedef struct atomic_i {
    int value;
    pthread_mutex_t *lock;
} Atomic_Int;

typedef struct thread_input {
    Atomic_Int *convArrayP;
    char *opsFile;
} Conv_Input;

void *perform_ops(void *arg);

int main(int argc, char *argv[]) {
    if(argc>=3) {
        int i;

        /* Create the shared memory array, initialize its int value
         * and lock */
        int convArraySize;
        sscanf(argv[1], "%d", &convArraySize);
        Atomic_Int *convArray[convArraySize];
        for (i = 0; i < convArraySize; i++) {
            convArray[i] = (Atomic_Int *) malloc(sizeof(Atomic_Int));
            convArray[i]->value = 0;
            convArray[i]->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
            pthread_mutex_init(convArray[i]->lock, NULL);
        }

        /* Initialize threads and run them */
        int threadCount = argc - 2;
        pthread_t *tid[threadCount];
        for (i = 0; i < threadCount; i++) {
            Conv_Input *thdArg = (Conv_Input *) malloc(sizeof(Conv_Input));
            thdArg->convArrayP = convArray;
            thdArg->opsFile = argv[i + 2];
        }

    }
    else
        printf("Please provide the appropriate number of command line arguments\n");

    exit(0);
}

/**
 * Reads in a file and performs the operations listed
 * within it on a shared array
 */
void *perform_ops(void *arg) {
    pthread_exit(NULL);
}
