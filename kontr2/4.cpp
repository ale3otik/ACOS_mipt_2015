
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/shm.h>
#include <sys/ipc.h>

#define CLIENT_SYNC 0
#define MAIN_SYNC 1
#define MAIN_SYNC_END 2
struct sembuf unlock_sem[3] = 
{	
	{CLIENT_SYNC,1,0},
	{MAIN_SYNC,1,0},
	{MAIN_SYNC_END,1,0}
};
struct sembuf lock_sem[3] = 
{
	{CLIENT_SYNC,-1,0},
	{MAIN_SYNC,-1,0},
	{MAIN_SYNC_END,-1,0}

};

char file_name[] = "key.txt";


int main()
{

	return 0;
}

void client()
{
	int * memory = NULL;
	int shm_id;
	int semaphore_id;
	key_t my_key;
	int a, b;
	my_key = ftok(file_name,1);

	semaphore_id = semget(my_key,3,0);

	semop(semaphore_id,&lock_sem[CLIENT_SYNC],1); /* sleep another clients*/

	shm_id = shmget(my_key, 2* sizeof(int),0);
	memory = (int *)shmat(shm_id,NULL,0);

	scanf("%d%d", &a,&b);
	memory[0] = a;
	memory[1] = b;
	semop(semaphore_id,&unlock_sem[MAIN_SYNC],1);/*let_server*/
	semop(semaphore_id,&lock_sem[MAIN_SYNC+1],1);/*wait server*/
	
	printf(" ans:: %d", memory[0]);

	semop(semaphore_id,&unlock_sem[CLIENT_SYNC],1); /*let another clients*/
}

void server()
{
	int * memory = NULL;
	int shm_id;
	int semaphore_id;
	key_t my_key;
	int check = 0;
	my_key = ftok(file_name,1);

	semaphore_id = semget(my_key,3,1);

	shm_id = shmget(my_key, 2*sizeof(int), IPC_CREAT|0666);
	memory = (int *)shmat(shm_id,NULL,0);
	
	while(1)
	{
		semop(semaphore_id,&lock_sem[MAIN_SYNC],1);
		check = memory[0];
		memory[0] = memory[1] - memory[0];
		semop(semaphore_id,&unlock_sem[MAIN_SYNC+1],1); /*let another clients*/
		if(check == 0) break;

	}
	
	semctl(semaphore_id,0,IPC_RMID);
	shmctl(shm_id, IPC_RMID, NULL);
}