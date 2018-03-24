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
    Atomic_Int **convArrayP;
    char *opsFile;
    int thread_id;
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
            thdArg->thread_id = i;
            tid[i] = (pthread_t *) malloc(sizeof(pthread_t));
            if (pthread_create(tid[i], NULL, perform_ops, (void *) thdArg)) {
                fprintf(stderr, "Error creating thread %d", i);
                exit(THREAD_CREATION_FAILED);
            }
        }

        for (i = 0; i < threadCount; i++) {
            if (pthread_join(*tid[i], NULL)) {
                fprintf(stderr, "Error joining thread %d", i);
                exit(THREAD_JOIN_FAILED);
            }
        }

        /* Print values of array */
        printf("Results:\n [");
        for (i = 0; i < convArraySize-1; i++) {
            printf("%d, ", convArray[i]->value);
        }
        printf("%d]\n", convArray[i]->value);
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
    Conv_Input *opsData = (Conv_Input *) arg;
    printf("Thread %d now processing %s\n", opsData->thread_id, opsData->opsFile);
    pthread_exit(NULL);
}
