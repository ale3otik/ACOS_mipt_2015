
/*PING PONG used semafors*/

#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define FIRST_TO_SECOND 0
#define SECOND_TO_THIRD 1
#define THIRD_TO_FIRST 2

pid_t pid_gamers[3];	
const int limit = 20;
const char sem_name[] = "ping_pong_sem_1";

struct sembuf unlock_sem[3] = 
{	
	{FIRST_TO_SECOND,1,0},
	{SECOND_TO_THIRD,1,0},
	{THIRD_TO_FIRST,1,0}
};
struct sembuf lock_sem[3] = 
{
	{FIRST_TO_SECOND,-1,0},
	{SECOND_TO_THIRD,-1,0},
	{THIRD_TO_FIRST,-1,0}	
};

void gamer_function(key_t my_key, int my_number)
{
	int * memory = NULL;
	int shm_id;
	int semaphore_id;

	printf("\nmy_num:<%d>\n",my_number);
	if(my_number == 0)
	{
		shm_id = shmget(my_key, sizeof(int), IPC_CREAT|0666);
		semaphore_id = semget(my_key,3,IPC_CREAT|0666);
	}
	else
	{
		shm_id = shmget(my_key, sizeof(int),0);
		semaphore_id = semget(my_key,3,0);
	}

	memory = (int *)shmat(shm_id,NULL,0);
	if(my_number == 0)
	{
		*memory = limit;
		semop(semaphore_id, &unlock_sem[SECOND_TO_THIRD],1);
	}
	

	while(1)
	{
		semop(semaphore_id,&lock_sem[my_number],1);
		if(*memory == 0) break;

		-- (*memory);
		printf("\nI'm %d - ball: %d ",my_number, *memory);
		semop(semaphore_id,&unlock_sem[(my_number+1) % 3],1);
	}

	printf("\n <%d> - ready",my_number);

	semop(semaphore_id,&unlock_sem[(my_number+1) % 3],1);
	if(my_number == 0)
	{
		semop(semaphore_id, &lock_sem[my_number], 1);
		semctl(semaphore_id,0,IPC_RMID);
		shmctl(shm_id, IPC_RMID, NULL);
	}

	exit(1);
}

void check_key_error(key_t my_key)
{
	if(my_key == -1)
	{
      	fprintf(stderr, "\nERROR: can't create key");
    	exit(1);
	}
}

int main()
{
	char file_name[] = "key_file.txt";
    int my_key = 0;
    int i;
    FILE * file_key = fopen(file_name, "w");
    
  	for(i = 0; i < 3; ++i)
  	{
  		if((pid_gamers[i] = fork()) == 0)
  		{
  			my_key = ftok(file_name,1);
        	check_key_error(my_key);

        	gamer_function(my_key,i);
  		}
  	}

  	/*...*/
  	for(i = 0;i<3;++i) wait(NULL);
  	fclose(file_key);
	return 0;
}
