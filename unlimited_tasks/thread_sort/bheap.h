
#ifndef _B_HEAP_H
#define _B_HEAP_H


#include <stdlib.h>
#include <stdio.h>
#include <memory.h>

struct BHeap
{
	char * data;
	size_t size;
	size_t capacity;	
};
typedef struct BHeap BHeap;
void heapify(int k , BHeap * heap);
void add_to_heap(char value , BHeap * heap);
int get_min(BHeap * heap);
void buildHeap(size_t max_size , BHeap * heap , char * new_data);

void heap_sort(char * array, size_t size);

#endif

