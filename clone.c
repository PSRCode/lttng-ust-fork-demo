#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <signal.h>
#define FIBER_STACK 8192

/*
 * We need to define TRACEPOINT_DEFINE in one C file in the program
 * before including provider headers.
 */
#define TRACEPOINT_DEFINE
#include "sample_component_provider.h"

void * stack;
int do_something(){
    while(1) {
	    printf("Hello World child\n");
	    tracepoint(sample_component, message, "Hello World child");
	    sleep(1);
    }
    return 0;
}

int main() {
    void * stack;
    stack = malloc(FIBER_STACK);
    if(!stack) {
        printf("The stack failed\n");
        exit(0);
    }

    int a = 0;
    clone(&do_something, (char *)stack + FIBER_STACK, 0, 0);
    while(1) {
	    printf("Hello World parent\n");
	    tracepoint(sample_component, message, "Hello World parent");
	    sleep(1);
    }

    free(stack);
    exit(1);
}

