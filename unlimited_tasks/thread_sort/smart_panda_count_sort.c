
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include "6_unlim_header.h"
static pthread_cond_t cond;
static pthread_mutex_t mutex;
static pthread_t reader;

static int fd;

static long the_great_array[256];
static unsigned char * buffer;
static int buf_size = -1;

volatile static short reader_in_mutex = 0;
volatile static short main_in_mutex = 0;
void * reader_func(void * arg)
{	
	while(1)
	{
		buffer = malloc(SIZE_OF_SORTED_PACK);
		if(buffer == NULL) errhand("MALLOC!");
		buf_size = read(fd,buffer,sizeof(char) * SIZE_OF_SORTED_PACK);

		if(buf_size < 0) errhand("read error");

		pthread_mutex_lock(&mutex);
			reader_in_mutex = 1;
			if(main_in_mutex)
			{
				pthread_cond_signal(&cond);
			}
			pthread_cond_wait(&cond,&mutex);
			reader_in_mutex = 0;
			pthread_cond_signal(&cond);
			pthread_cond_wait(&cond,&mutex);
		pthread_mutex_unlock(&mutex);

		if(buf_size <= 0) break;
	}
	return NULL;
}

int main(int argc, char ** argv)
{
	int i = 0;
	unsigned char c;
	unsigned char * tmp_to_write;
	unsigned char * my_buffer = NULL;
	int my_size = 0;

	if(argc <= 1) errhand("arguments!");
	for(i = 0; i < 256; ++i)
	{
		the_great_array[i] = 0;
	}

	fd = open(argv[1],O_RDWR);
	if(fd == -1) errhand("file!");

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);

	if(pthread_create(&reader,NULL,reader_func,NULL)) errhand("thread!");
/*****************************************************************/
	while(1)
	{
		pthread_mutex_lock(&mutex);
			main_in_mutex = 1;
			if(!reader_in_mutex)
			{
				pthread_cond_wait(&cond,&mutex);
			}
			my_buffer = buffer;
			my_size = buf_size;
			pthread_cond_signal(&cond);
			pthread_cond_wait(&cond,&mutex);
			main_in_mutex = 0;
			pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);

		if(my_size <= 0) my_size = 0;
		for(i = 0; i < my_size; ++i)
		{
			++the_great_array[my_buffer[i]];
		}

		free(my_buffer);
		if(my_size == 0)  break;

	}
/************************* Write Answer*******************************/
	pthread_join(reader,NULL);

	lseek(fd,0,SEEK_SET);
	for(c = 0; c < 255; ++c)
	{
		tmp_to_write = malloc(the_great_array[c]);
		for(i = 0; i < the_great_array[c]; ++i)
		{
			tmp_to_write[i] = c;
		}
		write(fd,tmp_to_write,the_great_array[c]);
		free(tmp_to_write);
	}

	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&mutex);

	close(fd);
	return 0;	
}