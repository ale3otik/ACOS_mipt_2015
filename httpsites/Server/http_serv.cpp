#include "main_header.h"
using namespace std;
map<int,connection_info> connections;
map<int,pthread_t> threads;
size_t next_connection_id = 0;
void * session_handler(void * id_ptr)
{
	const int max_client_buffer_size = 4096;
	char buf[max_client_buffer_size];	
	size_t id = (size_t)id_ptr;
	int fd = connections[id].fd;
	char * ip_str = connections[id].ip.str;
	while(1)
	{
		// cerr<<"!1";
		//int result = read(fd, buf, max_client_buffer_size);
		int result = recv(fd, buf, max_client_buffer_size, 0);
		// cerr<<"!2";
		if(result <= 0 )
		{
			cout<<"close connection: id: "<<id<<"ip: "<<ip_str<<endl;
			break;
			
		}

		buf[result] = 0;
		stringstream answer_full;
		process_query(answer_full,buf,result); //update 
		
		//result = write(fd, answer_full.str().c_str(),answer_full.str().length());
   		result = send(fd, answer_full.str().c_str(),answer_full.str().length(), 0);

    	if (result <= 0) 
    	{
      	  // send error
      		// printf("ERROR: send error to %ld in line:%d\n",id, __LINE__);
       		break;
    	}	
	}
	close(fd);
	threads.erase(id);
	connections.erase(id);
	pthread_exit(NULL);
}



/*****
**
*************MAIN******
**
*****/
int main()
{
	int socket_id = make_socket();
	bool is_error;
	int NUM_CONNECTIONS = get_config_int_options("NUM_CONNECTIONS", is_error);
	connection_info tmp_inf;
	if(is_error)
	{
		NUM_CONNECTIONS = STANDART_NUM_CONNECTIONS;
		cout<< "WARNING: can't read quantity of allowed connections, use standart: ";
	}
	else
	{
		cout<<"allowed quantity of connections: ";
	}
	cout<<NUM_CONNECTIONS<<endl;

	chdir("../Pages");
	
	struct sockaddr_in tmp_addr;
	int tmp_fd;
	socklen_t len;
	while(1)
	{	
		tmp_fd = tmp_inf.fd =accept(socket_id,(struct sockaddr *)&tmp_addr,&len);
		tmp_inf.ip.bytes = tmp_addr.sin_addr.s_addr;
		strcpy(tmp_inf.ip.str,inet_ntoa(tmp_addr.sin_addr));
		tmp_inf.id = next_connection_id;
		++next_connection_id;
		
		cout<< "Conected: "<<tmp_inf.ip.str << "id: "<< tmp_inf.id<<endl;
		connections[tmp_inf.id] = tmp_inf;
		if(pthread_create(&threads[tmp_inf.id],NULL,session_handler,(void*)tmp_inf.id))
		{
			cerr<<"ERROR!:Can't create thread\n";
			close(tmp_fd);
			continue;
		}
		
	}
	close(socket_id);
	cerr<<"EXIT PROGRAMM\n";
	return 0;
}
