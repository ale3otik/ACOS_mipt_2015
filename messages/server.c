#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>


#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include <memory.h>

#define TCP_PORT_NUMBER 5556
#define NUM_CONNECTIONS 25  
#define MAX_LOGIN_LENGTH 20
#define MAX_MSG_SIZE 1024

struct client_info
{
	int fd;
	int numconn;
};

void *handler(void *args);
pthread_t thread;
int id_thread;

struct client_info connections[NUM_CONNECTIONS];
char logins[30][MAX_MSG_SIZE];
struct sockaddr_in clients[NUM_CONNECTIONS];

pthread_t threads[NUM_CONNECTIONS];

pthread_mutex_t body_mutex;
pthread_mutex_t mutex;

int num_connected=0;

int main(void)
{
	int socket_id;
	struct protoent *protocol_record=NULL;
	struct sockaddr_in server_address;

	socklen_t len;

	int i;

	pthread_mutex_init(&mutex,NULL);

	protocol_record=getprotobyname("tcp");
	/* printf("protocol id is : %d\n",protocol_record->p_proto); */
	socket_id=socket(PF_INET,SOCK_STREAM,protocol_record->p_proto);

	server_address.sin_family=AF_INET;
	server_address.sin_port=htons(TCP_PORT_NUMBER);
	server_address.sin_addr.s_addr=INADDR_ANY;

	if(bind(socket_id,(struct sockaddr *)&server_address,sizeof(struct sockaddr_in)))
	{
		printf("ERRR\n");
		return -1;
	}

	listen(socket_id,30);
	
	for(i=0;i<NUM_CONNECTIONS;i++)
	{
		connections[i].fd=-1;
		connections[i].numconn = i;
	}
	
	for(;;)
	{
		struct sockaddr_in tmp_addr;
		int tmp_fd;

		if(num_connected<NUM_CONNECTIONS)
		{
			
			for(i=0;i<NUM_CONNECTIONS;i++)
			{
				if(connections[i].fd==-1)
				{
					break;
				}
			}
		
		
			tmp_fd=accept(socket_id,(struct sockaddr *)&clients[i],&len);	
			connections[i].fd = tmp_fd;

			pthread_mutex_lock(&mutex);
				num_connected++;
			pthread_mutex_unlock(&mutex);
			

			if(pthread_create(&threads[i],NULL,handler,&connections[i]))
			{
				printf("Can't create thread\n");

				close(socket_id);
				return -1;
			}

		}
		else
		{
			tmp_fd=accept(socket_id,(struct sockaddr *)&tmp_addr,&len);
			write(tmp_fd,"Server.  Buisy. \n",18);
			close(tmp_fd);
		}
	}	

	return 0;

}
/*
void* body(void* param)
{
	char character[1024];
	size_t size = 0;
	int fd = *(int*)param;
	while(1)
	{
		//pthread_mutex_lock(&body_mutex);
			if((size = read(0, character, 1024)) == 0)
				break;
		//pthread_mutex_unlock(&body_mutex);
		write(fd,character, size);
		//write(1,&character, 1);
	}
	return;
}
*/
void *handler(void *args)
{
	int me_num = ((struct client_info*)args)->numconn;
	int fd= ((struct client_info*)args)->fd;
	int size = 0;
	int login_size = 0;
	int i = 0;
	char character[900];
	char msg[1024];
	write(1, "OK Connect\n", 12);
	write(fd,"Server. Welcome.\n",18);
	
	id_thread = fd;
	
	login_size = read(fd, logins[me_num], 100);
	logins[me_num][login_size-1] = 0;


	while(1)
	{
		size = read(fd,character,1024);
		if(size <= 0) break;
		/*pthread_mutex_lock(&body_mutex);*/
			memcpy(msg,logins[me_num], login_size);
			msg[login_size] = ':';
			msg[login_size + 1] = ' ';
			memcpy(msg + login_size + 2,character, size);
			msg[login_size + size + 2] = (char)login_size;
			for(i = 0;i < NUM_CONNECTIONS;++i)
			{
				if(connections[i].fd != -1)
				{
					write(connections[i].fd, msg, login_size + size + 3);
				}
			}
		/*pthread_mutex_unlock(&body_mutex);*/
	}
	connections[i].fd = -1;
	close(fd);
	write(1, "OK Disconnect\n", 15);
	pthread_mutex_lock(&mutex);
		((struct client_info*)args)->fd = -1;
		num_connected--;
	pthread_mutex_unlock(&mutex);

	return NULL;
}


