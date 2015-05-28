#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>


#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#define TCP_PORT_NUMBER 5555
#define NUM_CONNECTIONS 2  
#define SIZE_MSG 100
int socket_id;
int connections_fd[NUM_CONNECTIONS];
struct sockaddr_in clients[NUM_CONNECTIONS];
pthread_t threads[NUM_CONNECTIONS];
pthread_mutex_t mutex;
socklen_t len;
char ** names;
int count_exit = 0;

void * delete_all()
{
	close(socket_id);
	close(connections_fd[0]);
	close(connections_fd[1]);
	pthread_mutex_destroy(&mutex);
	exit(1);
}

void * handler(void * my_ind_get)
{
	char * read_buffer = NULL;
	int my_ind = (long)my_ind_get;
	long size = 0;

	FILE * fp = fopen(names[my_ind],"w");
	while(1)
	{
		read_buffer = (char *) malloc(SIZE_MSG);
		if(read_buffer = NULL) delete_all();
		size  = read(connections_fd[my_ind],read_buffer,sizeof(read_buffer));
		
		if(size == 0)
		{
			pthread_mutex_lock(&mutex);
			count_exit++;
			fclose(fp);
			if(count_exit == NUM_CONNECTIONS) delete_all();
			pthread_mutex_unlock(&mutex);
			break;
		}
		fwrite(read_buffer,sizeof(char),size,fp);
	}

	return NULL;
}


int main(int argc, char ** argv)
{
	
	struct protoent *protocol_record=NULL;
	struct sockaddr_in server_address;
	int last_fd;
	socket_id=socket(PF_INET,SOCK_STREAM,protocol_record->p_proto);

	if(argc < 3) exit(1);
	names = argv;

	pthread_mutex_init(&mutex, NULL);
	server_address.sin_family=AF_INET;
	server_address.sin_port=htons(TCP_PORT_NUMBER);
	server_address.sin_addr.s_addr=INADDR_ANY;

	if(bind(socket_id,(struct sockaddr *)&server_address,sizeof(struct sockaddr_in)))
	{
		printf("ERRR\n");
		return -1;
	}
	listen(socket_id,5);

		for(int i = 0; i < NUM_CONNECTIONS; ++i)
		{
			connections_fd[i] = accept(socket_id,(struct sockaddr *)&clients[i],&len);
			if(pthread_create(&threads[i],NULL,handler,(void*)i))
			{
				printf("Can't create thread\n");

				close(socket_id);
				return -1;
			}
		}

		last_fd = accept(socket_id,NULL,&len);
		write(last_fd,"you looooose!",sizeof("you looooose!"));
		close(last_fd);

	pthread_join(threads[0],NULL);
	pthread_join(threads[1],NULL);

	return 0;
}