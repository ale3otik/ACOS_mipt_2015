
#include "6_unlim_header.h"
/********************************************************/

static pthread_t sorters[NUM_OF_THREADS];
static pthread_t reader; /* to read the file and send to main() */
static pthread_t writer; /* to get sorted packs from sorters and write into file */

static pthread_mutex_t mutex_ready_sorter[2];/* 1 to lock main thread and the second to lock brothers*/
static pthread_mutex_t mutex_ready_to_write[2];
static pthread_mutex_t mutex_for_reader;
static pthread_mutex_t mutex_file_acces;

static pthread_cond_t cond_ready_sorter;
static pthread_cond_t cond_ready_to_write;
static pthread_cond_t cond_for_reader;
/*static pthread_cond_t cond_file_acces;*/

static char * reader_buffer = NULL;
static long read_size = 0;
static size_t file_range[2];

static int fd = 0;

static char * work_for_sorter;
static long size_for_sorter = -1;
static size_t worker_file_range[2];

static char * array_to_print = NULL;
static long size_to_write = 0;
static size_t write_file_range[2];

static int writer_is_killed = 0;

volatile static char main_in_reader_mutex = 0;
volatile static char main_in_sorter_mutex = 0;
volatile static char reader_in_mutex = 0;
volatile static char writer_in_mutex = 0;
volatile static char sorter_in_writer_mutex = 0;
volatile static char sorter_in_main_mutex = 0;

static long length_of_file = 0;
void * thread_sort_function(void * who_am_i)
{
	char * my_work;
	size_t my_work_size;
	size_t my_file_range[2];
	while(1)
	{
		stateprint("sorter 0 : start");
		pthread_mutex_lock(&mutex_ready_sorter[0]); /*stop other threads*/
			pthread_mutex_lock(&mutex_ready_sorter[1]); /*sync main*/
			sorter_in_main_mutex = 1;
			if(!main_in_sorter_mutex)
			{
				pthread_cond_wait(&cond_ready_sorter,&mutex_ready_sorter[1]);	
			}
			
			my_work = work_for_sorter;
			my_work_size = size_for_sorter;
			my_file_range[0] = worker_file_range[0];
			my_file_range[1] = worker_file_range[1];

			sorter_in_main_mutex = 0;
			pthread_cond_signal(&cond_ready_sorter);
			pthread_cond_wait(&cond_ready_sorter,&mutex_ready_sorter[1]);	

			pthread_mutex_unlock(&mutex_ready_sorter[1]); /* let another threads get works*/

		pthread_mutex_unlock(&mutex_ready_sorter[0]);

		heap_sort(my_work, my_work_size);

/*WRITE TO FILE*/
			pthread_mutex_lock(&mutex_ready_to_write[0]); /*lock other threads*/
			
			pthread_mutex_lock(&mutex_ready_to_write[1]);
				sorter_in_writer_mutex = 1;

				array_to_print = my_work;
				size_to_write = my_work_size;
				write_file_range[0] = my_file_range[0];
				write_file_range[1] = my_file_range[1];

				if(writer_in_mutex)
				{
					pthread_cond_signal(&cond_ready_to_write);
				}
			
			pthread_cond_wait(&cond_ready_to_write,&mutex_ready_to_write[1]);
			sorter_in_writer_mutex = 0;
			pthread_cond_signal(&cond_ready_to_write);

			pthread_mutex_unlock(&mutex_ready_to_write[1]);

			pthread_mutex_unlock(&mutex_ready_to_write[0]); /*unlock other threads*/
			stateprint("sorter 2: end");
		
		if(my_work_size == 0) break;
	}
	return NULL;
}

void * thread_reader_function(void * arg)
{
	static short is_end_find = 0;
	static long offset = 0;
	
	while(1)
	{
		reader_buffer = (char*)malloc(sizeof(char) * SIZE_OF_SORTED_PACK);

		if(is_end_find)
		{
				read_size = 0;
				break;
		}
		else
		{
			pthread_mutex_lock(&mutex_file_acces);
				stateprint("reader: 0");
				lseek(fd,offset,SEEK_SET);
				read_size = read(fd,reader_buffer,SIZE_OF_SORTED_PACK * sizeof(char));
			pthread_mutex_unlock(&mutex_file_acces);

			offset += read_size;
			length_of_file += read_size;

			file_range[0] = file_range[1];
			fprintf(stderr,"\nread_size: %ld ; range : %ld %ld", read_size, file_range[0], file_range[1]+read_size);
			if(read_size == 0)
			{
				stateprint("reader:null size");
				free(reader_buffer);
				reader_buffer = NULL;
				is_end_find = 1;
			}

			file_range[1] += read_size;
		}
		pthread_mutex_lock(&mutex_for_reader); /*wait when main_thread ready to get buffer*/
		reader_in_mutex = 1;
	
		if(main_in_reader_mutex) 
		{
			stateprint("send cond to main");
			pthread_cond_signal(&cond_for_reader);/*let main do work*/
		}

		stateprint("reader: 3 wait main");

		pthread_cond_wait(&cond_for_reader,&mutex_for_reader);

		stateprint("reader: 4 end wait "); 
		
		reader_in_mutex = 0;
		pthread_cond_signal(&cond_for_reader);
		pthread_mutex_unlock(&mutex_for_reader); /* let main get results*/

		if(is_end_find) break;
	}

	return NULL;
}	

void * thread_writer_function(void * arg)
{
	static long offset = 0;

	char * my_data_to_write;
	long my_length_to_write;
	size_t my_write_range[2];
	int count_sorter = 0;
	while(1)
	{
		pthread_mutex_lock(&mutex_ready_to_write[1]); /* wait sorter*/
			writer_in_mutex = 1;
			stateprint("writer 1: in writer!");
			if(!sorter_in_writer_mutex)
			{
				pthread_cond_wait(&cond_ready_to_write,&mutex_ready_to_write[1]);
			}
			
			my_data_to_write = array_to_print;
			my_length_to_write = size_to_write;
			my_write_range[0] = write_file_range[0];
			my_write_range[1] = write_file_range[1];

			writer_in_mutex = 0;
			pthread_cond_signal(&cond_ready_to_write);
			pthread_cond_wait(&cond_ready_to_write,&mutex_ready_to_write[1]);		
		pthread_mutex_unlock(&mutex_ready_to_write[1]); /* let sorter working*/
	
		if(my_length_to_write == 0)
		{
			
			count_sorter++;
			fprintf(stderr, "\nA:::count_sorter%d:::A\n",count_sorter);
			if(count_sorter == NUM_OF_THREADS)
			{
				writer_is_killed = 1;
				break;
			}
		}

		pthread_mutex_lock(&mutex_file_acces);
			if(my_length_to_write > 0)
			{
				lseek(fd,my_write_range[0],SEEK_SET);
				write(fd, my_data_to_write ,sizeof(char) *  my_length_to_write);
			}
		pthread_mutex_unlock(&mutex_file_acces);

		if(my_data_to_write != NULL && my_length_to_write != 0)
			free(my_data_to_write);
	}

	return NULL;
}

int main(int argc , char ** argv)
{
	short is_creating_error = 0;
	long i = 0;
	int count_of_exit_threads = 0;

	if(argc < 2)
	{
		errhand("wrong quantity of arguments");
	}

	fd = open(argv[1],O_RDWR);
	if(fd < 0) errhand("file openning error");

	/*for(i = 0; i < NUM_OF_THREADS; ++i)
	{
		pthread_mutex_init(&mutex_array[i],NULL);
	}*/
	pthread_mutex_init(&mutex_for_reader, NULL);
	pthread_mutex_init(&mutex_ready_sorter[0], NULL);
	pthread_mutex_init(&mutex_ready_sorter[1], NULL);
	pthread_mutex_init(&mutex_ready_to_write[1], NULL);
	pthread_mutex_init(&mutex_ready_to_write[1], NULL);
	pthread_mutex_init(&mutex_file_acces,NULL);

	pthread_cond_init(&cond_ready_sorter,NULL);
	pthread_cond_init(&cond_ready_to_write,NULL);
	pthread_cond_init(&cond_for_reader,NULL);
	

	stateprint("main: mutex inited");

	for(i = 0;i < NUM_OF_THREADS; ++i)
	{
		if(pthread_create(&sorters[i],NULL,thread_sort_function,(void *)i)) 
		{
			stateprint("create error");
			is_creating_error = 1;
			break;
		}
	}		
	if( pthread_create(&reader,NULL,thread_reader_function,NULL) ||
		pthread_create(&writer,NULL,thread_writer_function,NULL))
	{
		stateprint("create error");
		is_creating_error = 1;
	}
	
	printf("\n%d",is_creating_error);
	if(is_creating_error) errhand("Can't create thread");

	stateprint("main: threads created");

	/* main body*/

	while(count_of_exit_threads < NUM_OF_THREADS)
	{
		stateprint("main : 1 print while");
		if(size_for_sorter != 0)
		{
			pthread_mutex_lock(&mutex_for_reader); /* wait next data from reader*/
			main_in_reader_mutex = 1;
				if(!reader_in_mutex)
				{
					stateprint("main 2:wait reader");
					pthread_cond_wait(&cond_for_reader,&mutex_for_reader);/*wait reader*/
					stateprint("main 3 : end wait");
				}
				
				work_for_sorter = reader_buffer;
				size_for_sorter = read_size;
				worker_file_range[0] = file_range[0];
				worker_file_range[1] = file_range[1];
				stateprint("main 4 : end read");

				main_in_reader_mutex = 0;
				pthread_cond_signal(&cond_for_reader);/*let reader*/
				pthread_cond_wait(&cond_for_reader,&mutex_for_reader);
			pthread_mutex_unlock(&mutex_for_reader); /* let reader get next data*/
		}
		else
		{
			size_for_sorter = 0;
		}

		if(size_for_sorter == 0) ++count_of_exit_threads;

		stateprint("main 5 : end of read");

		pthread_mutex_lock(&mutex_ready_sorter[1]); /* let sorter copy work*/
			main_in_sorter_mutex = 1;
			
			if(sorter_in_main_mutex)
			{
				pthread_cond_signal(&cond_ready_sorter);	
			}			

			pthread_cond_wait(&cond_ready_sorter,&mutex_ready_sorter[1]);
			main_in_sorter_mutex = 0;
			pthread_cond_signal(&cond_ready_sorter);	

		pthread_mutex_unlock(&mutex_ready_sorter[1]); /* wait sorter*/

		stateprint("main 6: end of all");
	}

	/* end */
	stateprint("main inf: wait exit");
	for(i = 0;i < NUM_OF_THREADS; ++i)
	{
		pthread_join(sorters[i],NULL);
	}
	stateprint("sorters closed");

	pthread_join(reader,NULL);
	stateprint("reader closed");
	pthread_cancel(writer);	
	/*pthread_join(writer,NULL);*/
	stateprint("writer closed");

	pthread_mutex_destroy(&mutex_for_reader);
	pthread_mutex_destroy(&mutex_ready_sorter[0]);
	pthread_mutex_destroy(&mutex_ready_sorter[1]);
	pthread_mutex_destroy(&mutex_ready_to_write[0]);
	pthread_mutex_destroy(&mutex_ready_to_write[1]);
	pthread_mutex_destroy(&mutex_file_acces);

	pthread_cond_destroy(&cond_ready_sorter);
	pthread_cond_destroy(&cond_ready_to_write);
	pthread_cond_destroy(&cond_for_reader);

	close(fd);

	return length_of_file;
}






