#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <netdb.h>
#include <signal.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STR_SIZE 1024
#define MAX_ADDR_LEN 1024
#define MAX_NUMBER_LEN 6
#define MAX_LOGIN_NAME 20
const int standart_tcp_port_number = 5555;
const char standart_server_addr[] = "hill.cs.msu.ru";
const char config_path[] = "config.cfg";
int tcp_port_number;
char server_addr[MAX_ADDR_LEN];
pthread_t thread;
char str[STR_SIZE];
pthread_mutex_t mutex;
char tmp_str[STR_SIZE];
short is_need_update = 0;
int socket_id;
struct termios old_term, new_term;
volatile int current_printed_seek = 0;
volatile int ind = -1;

void destroy()
{
	pthread_cancel(thread);
	close(socket_id);
	pthread_mutex_destroy(&mutex);
	tcsetattr( STDIN_FILENO, TCSANOW, &old_term );
	exit(0);
}

void sighand(int sig)
{
	destroy();
}

void get_attr()
{
	char check_name = 0;
	char check_port = 0;
	char port_str[MAX_ADDR_LEN + 1];
	FILE * fp = fopen(config_path,"r");
	printf("\nread config.cfg...");
	if(fp == NULL)
	{
		check_name = 1;
		printf("\ncan't open config.cfg");
	}
	else
	{
		if(fgets(server_addr,MAX_ADDR_LEN-1,fp) == NULL) 
		{
			check_name = 1;
		}
		else
			server_addr[strlen(server_addr) - 1] = 0;
		{
			if(fgets(port_str,MAX_NUMBER_LEN,fp) != NULL)
			{
				int i;
				for(i = 0 ;i < MAX_NUMBER_LEN && port_str[i] != 0; ++i)
				{
					if(port_str[i]<'0' || port_str[i] > '9')
					{
						check_port = 1;
						break;
					}
				}
			}
			else
			{
				check_port = 1;
			}
		}
	}

	if(check_name == 1)
	{
		printf("\ncan't read addres and port \n use standart port and address");		
		memcpy(server_addr, standart_server_addr, strlen(standart_server_addr));
		tcp_port_number = standart_tcp_port_number; 
	}
	else
	{
		if(check_port == 1)
		{
			tcp_port_number = standart_tcp_port_number; 		
			printf("\ncan't read port number\nuse standart 5555 port number");
		}
		else
			tcp_port_number = atoi(port_str);
	}
}

void free_str2(int value)
{
	int i;
	 for(i = 0; i < value; ++i)
	 {
	 	printf("\b");
	 	printf(" ");
	 	printf("\b");
	 }
}

void print_str2()
{
	printf("%s",str);
}

void free_str1(int value)
{
	int i;
	 for(i = 0; i < value; ++i)
	 {
	 	printf("\b");
	 	printf(" ");
	 	printf("\b");
	 }
}

void print_str1()
{
	printf("%s",str);
}

void * reader(void * arg)
{
	int id = *((int*) arg);
	long r_size = 0;
	char login_size;
	char  my_str[STR_SIZE+1];
	char login_str[MAX_LOGIN_NAME];
	while(1)
	{
		r_size = read(id, my_str,STR_SIZE);
		if(r_size <= 0) break;
		my_str[r_size] = 0;
		/*printf(".%s",my_str);*/

		login_size = my_str[r_size - 1];
		login_str[r_size - 1] = 0;
		memcpy(login_str, my_str, login_size);

		pthread_mutex_lock(&mutex);
		free_str2(current_printed_seek);

		printf("\033[31m"); 
		printf("%s",login_str);

		printf("\033[32m"); 
		printf("%c", my_str[(long)login_size]);
		printf("\033[0m"); 
		printf("%s",my_str + login_size + 1);
		fprintf(stderr,"%s",str);
		
		/*print_str2();*/

		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void writer()
{
	while(1)
	{
		int c;
		str[0] = 0;
		for(ind = 0; ind < SIZE_MAX-1; ++ind)
		{
			c = getchar();
			if(c == '\n' || feof(stdin)) break;
			else if(c == '\b' || c == 127)
			{
				if(ind > 0)
				{
					--ind;
					str[ind] = 0;
					--ind;
					pthread_mutex_lock(&mutex);
					free_str1(current_printed_seek);
					print_str1();
					current_printed_seek = ind+1;
					pthread_mutex_unlock(&mutex);
				}
				else --ind;
			} 
			else if((c>= 32 && c<= 255))
			{
				pthread_mutex_lock(&mutex);
				free_str1(current_printed_seek);
				str[ind] = c;
				str[ind+1] = 0;
				print_str1();
				current_printed_seek = ind + 1;
				pthread_mutex_unlock(&mutex);
			}
			else --ind;
		}

		pthread_mutex_lock(&mutex);
		free_str1(current_printed_seek);
		current_printed_seek = 0;

		str[ind] = '\n';
		str[ind+1] = 0;
		ind = -1;
		if(strlen(str) <= 1) continue;

		write(socket_id,str,strlen(str));
		str[0] = 0;
		pthread_mutex_unlock(&mutex);
	}
}

int main(int argc,char **argv)
{
	struct protoent *protocol_record=NULL;
	struct hostent *host_record=NULL;
	struct sockaddr_in server_address;
	int sock_check;
	char login[MAX_LOGIN_NAME + 1];

fprintf(stderr,"version 3");

	get_attr(); /*read config cfg*/
	printf("adr:%s port:%d", server_addr , tcp_port_number);

	printf("\nEnter your login: ");

	fgets(login, MAX_LOGIN_NAME, stdin);

	tcgetattr( STDIN_FILENO, &old_term);
	new_term = old_term;
    new_term.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &new_term );

	signal(SIGINT,sighand);

	pthread_mutex_init(&mutex,NULL);
	protocol_record=getprotobyname("tcp");
	/* printf("protocol id is : %d\n",protocol_record->p_proto); */
	socket_id=socket(PF_INET,SOCK_STREAM,protocol_record->p_proto);
		
	host_record=gethostbyname(server_addr);
	if(host_record==NULL)
	{
		printf("Can't get address by name '%s'\n",argv[1]);
		return -1;
	}


	server_address.sin_family=AF_INET;
	server_address.sin_port=htons(tcp_port_number);
    
	memcpy
	(
	 	&server_address.sin_addr.s_addr,
		host_record->h_addr_list[0],
		host_record->h_length
	);
	
	printf("connecting to server\n");
	sock_check=
	connect
	(
	 	socket_id,
		(struct sockaddr *)&server_address,
		sizeof(struct sockaddr_in)
	);

	if(sock_check)
	{
		printf("Can't connect to server\n");
		return -1;
	}

	read(socket_id,str,18);
	str[16]='\0';
	if(strcmp(str,"Server. Welcome."))
	{
		printf("Incorrect server invatation\n");
		printf("Server tells '%s'\n",str);
		return -1;
	}

	write(socket_id,login,strlen(login));

	printf("\n%s",str);
	printf("server connected\n");

	if(pthread_create(&thread,NULL,reader,&socket_id))
	{
			printf("Can't create thread\n");

			close(socket_id);
			return -1;
	}

	writer();

	destroy();
	return 0;
}
