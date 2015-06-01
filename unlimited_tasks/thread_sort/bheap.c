/*HEAP*/
#include "bheap.h"
void delete_heap(BHeap * heap)
{
	if(heap == NULL) return;
	if(heap -> data != NULL) free(heap -> data);
}

void build_heap(size_t max_size, BHeap * heap , char * new_data)
{
	long i = 0;

	heap -> capacity = max_size;
	heap -> size = max_size;
	heap -> data = (char*)malloc(sizeof(char) * max_size);
	if(heap->data == NULL)
	{
		fprintf(stderr,"\nERROR:heap allocation");
		exit(1);
	}

	memcpy(heap->data, new_data, max_size * sizeof(char));

	for (i = max_size / 2; i >= 0; i--)
	{
		heapify(i , heap);
	}
}

void heapify(int k , BHeap * heap)
{
	int leftChild;
	int rightChild;
	int maxChild,parent = k;
	size_t size = heap-> size;
	char * data = heap -> data;
	while (1)
	{
	    leftChild = parent * 2 + 1;
	    rightChild = parent * 2 + 2;
		if (leftChild >= size)
		{
			return;
		}
		if (rightChild >= size)
		{
			rightChild = -1;
		}
		if (rightChild != -1)
		{
			if (data[leftChild] < data[rightChild]) maxChild = leftChild;
			else maxChild = rightChild;
		}
		else
		{
			maxChild = leftChild;
		}
		if (data[maxChild] < data[parent])
		{
			char buf = data[maxChild];
			data[maxChild] =  data[parent];
			data[parent] = buf;
			parent = maxChild;
		}
		else return;
	}
}

void add_to_heap(char value , BHeap * heap)
{
	size_t size = heap -> size;
	char * data = heap -> data;
	int parent, child = size;
	heap ->size ++;
	data[child] = value;
	while (1)
	{
		parent = (child - 1) / 2;
		if (parent < 0 || parent == child) return;
		if (data[child] < data[parent])
		{
			char buf = data[parent];
			data[parent] = data[child];
			data[child] = buf;
		}
		else return;
		child = parent;
	}
}


int get_min(BHeap * heap)
{
	size_t size = heap -> size;
	char * data = heap -> data;
	char deleted_el = data[0];
	
	data[0] = data[size - 1];
	--heap->size;
	heapify(0 , heap);
	
	return deleted_el;
}
/**************heap sort****************/
void heap_sort(char * array, size_t size)
{
	BHeap heap;
	long i;

	if( size == 0) return;

	build_heap(size,&heap,array);
	for(i = 0 ; i < size; ++i)
	{
		array[i] = get_min(&heap);
	}

	delete_heap(&heap);
}

