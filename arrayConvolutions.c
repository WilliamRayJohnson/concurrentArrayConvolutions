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

int main(int argc, char *argv[]) {
    if(argc>=3) {
        printf("You provided %d arguments\n", argc);
    }
    else
        printf("Please provide the appropriate number of command line arguments\n");

    exit(0);
}
