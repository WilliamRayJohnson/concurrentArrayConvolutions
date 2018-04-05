
/*
 * A concurrent program that perform arithmetic operations on an array of
 * a given size whilst maintaining atomicity on each element of the array.
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

#define THREAD_CREATION_FAILED -1
#define THREAD_JOIN_FAILED -2
#define MAX_OP_SIZE 5


typedef struct atomic_i {
    int value;
    pthread_mutex_t *lock;
    pthread_mutexattr_t *attr;
} Atomic_Int;

typedef struct thread_input {
    char *opsFile;
    int thread_id;
} Conv_Input;

void *perform_ops(void *arg);

//Shared values for threads to access
Atomic_Int *convArray;

int main(int argc, char *argv[]) {
    if(argc>=3) {
        int i;

        /* Create the shared memory array, initialize its int value
         * and lock */
        int convArraySize;
        sscanf(argv[1], "%d", &convArraySize);
        convArray = (Atomic_Int *) malloc(convArraySize * sizeof(Atomic_Int));
        for (i = 0; i < convArraySize; i++) {
            convArray[i].value = 0;
            convArray[i].lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
            convArray[i].attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
            pthread_mutexattr_init(convArray[i].attr);
            pthread_mutexattr_settype(convArray[i].attr, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(convArray[i].lock, convArray[i].attr);
        }

        /* Initialize threads and run them */
        int threadCount = argc - 2;
        pthread_t *tid[threadCount];
        for (i = 0; i < threadCount; i++) {
            Conv_Input *thdArg = (Conv_Input *) malloc(sizeof(Conv_Input));
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
            printf("%d, ", convArray[i].value);
        }
        printf("%d]\n", convArray[i].value);
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

    FILE *fp;
    fp = fopen(opsData->opsFile, "r");
    char currentOp;
    int leftOperandi;
    int rightOperand;
    char endOfLine;
    char rightOpCheck;
    bool rightOpIsIndex = false;
    int i;
    int expBase;
    do {
        i = 0;
        rightOpIsIndex = false;
        /* Read in operation */
        fscanf(fp, "%d", &leftOperandi);
        fscanf(fp, "%c", &currentOp);
        rightOpCheck = fgetc(fp);
        /* Check if right operand is an index */
        if(rightOpCheck == 'i')
            rightOpIsIndex = true;
        else
            ungetc(rightOpCheck, fp);
        fscanf(fp, "%d", &rightOperand);
        endOfLine = fgetc(fp);

        /* Atomically perform operation */
        if(endOfLine != EOF) {
            switch(currentOp) {
                case '+' :
                    if(rightOpIsIndex) {
                        pthread_mutex_lock(convArray[leftOperandi].lock);
                        pthread_mutex_lock(convArray[rightOperand].lock);

                        convArray[leftOperandi].value += convArray[rightOperand].value;

                        pthread_mutex_unlock(convArray[rightOperand].lock);
                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    else {
                        pthread_mutex_lock(convArray[leftOperandi].lock);

                        convArray[leftOperandi].value += rightOperand;

                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    break;
                case '-' :
                    if(rightOpIsIndex) {
                        pthread_mutex_lock(convArray[leftOperandi].lock);
                        pthread_mutex_lock(convArray[rightOperand].lock);

                        convArray[leftOperandi].value -= convArray[rightOperand].value;

                        pthread_mutex_unlock(convArray[rightOperand].lock);
                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    else {
                        pthread_mutex_lock(convArray[leftOperandi].lock);

                        convArray[leftOperandi].value -= rightOperand;

                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    break;
                case '*' :
                    if(rightOpIsIndex) {
                        pthread_mutex_lock(convArray[leftOperandi].lock);
                        pthread_mutex_lock(convArray[rightOperand].lock);

                        convArray[leftOperandi].value *= convArray[rightOperand].value;

                        pthread_mutex_unlock(convArray[rightOperand].lock);
                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    else {
                        pthread_mutex_lock(convArray[leftOperandi].lock);

                        convArray[leftOperandi].value *= rightOperand;

                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    break;
                case '/' :
                    if(rightOpIsIndex) {
                        pthread_mutex_lock(convArray[leftOperandi].lock);
                        pthread_mutex_lock(convArray[rightOperand].lock);

                        if(convArray[rightOperand].value != 0)
                            convArray[leftOperandi].value /= convArray[rightOperand].value;
                        else
                            printf("Thread %d performed an illegal OP: division by 0\n", opsData->thread_id);
                        pthread_mutex_unlock(convArray[rightOperand].lock);
                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    else {
                        pthread_mutex_lock(convArray[leftOperandi].lock);

                        if(rightOperand != 0)
                            convArray[leftOperandi].value /= rightOperand;
                        else
                            printf("Thread %d performed an illegal OP: division by 0\n", opsData->thread_id);

                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    break;
                case '^' :
                    expBase = 0;
                    if(rightOpIsIndex) {
                        pthread_mutex_lock(convArray[leftOperandi].lock);
                        pthread_mutex_lock(convArray[rightOperand].lock);

                        if(convArray[leftOperandi].value != 0 && convArray[rightOperand].value != 0) {
                            expBase = convArray[leftOperandi].value;
                            for(i = 1; i < abs(convArray[rightOperand].value); i++)
                                convArray[leftOperandi].value *= expBase;
                            //Check for negative exponent and 0 exponent
                            if(convArray[rightOperand].value < 0 && convArray[leftOperandi].value != 0)
                                convArray[leftOperandi].value = 1/convArray[leftOperandi].value;
                            else if(convArray[leftOperandi].value == 0)
                                printf("Thread %d performed an illegal OP: division by 0\n", opsData->thread_id);
                            else if(convArray[rightOperand].value == 0)
                                convArray[leftOperandi].value = 1;
                        }
                        else
                            printf("Thread %d performed an illegal OP: 0^0\n", opsData->thread_id);

                        pthread_mutex_unlock(convArray[rightOperand].lock);
                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    else {
                        pthread_mutex_lock(convArray[leftOperandi].lock);

                        if(convArray[leftOperandi].value != 0 && rightOperand != 0) {
                            expBase = convArray[leftOperandi].value;
                            for(i = 1; i < abs(rightOperand); i++)
                                convArray[leftOperandi].value *= expBase;
                            //Check for negative exponent and 0 exponent
                            if(rightOperand < 0 && convArray[leftOperandi].value != 0)
                                convArray[leftOperandi].value = 1/convArray[leftOperandi].value;
                            else if(convArray[leftOperandi].value == 0)
                                printf("Thread %d performed an illegal OP: division by 0\n", opsData->thread_id);
                            else if(rightOperand == 0)
                                convArray[leftOperandi].value = 1;
                        }
                        else
                            printf("Thread %d performed an illegal OP: 0^0\n", opsData->thread_id);

                        pthread_mutex_unlock(convArray[leftOperandi].lock);
                    }
                    break;
                default :
                    printf("Op not considered: %c\n", currentOp);
            }
        }

    } while (endOfLine != EOF);

    pthread_exit(NULL);
}
