#ifndef _UNLIM_PTHREAD_SORT_HEADER_
#define _UNLIM_PTHREAD_SORT_HEADER_

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

#include "bheap.h"
#define errhand(n) {fprintf(stderr, "\nERROR: %s\n" ,n ); exit(1);}
#define stateprint(n) fprintf(stderr,"\nSTATE: %s\n", n )
#define NUM_OF_THREADS 10
#define SIZE_OF_SORTED_PACK 10
#endif
