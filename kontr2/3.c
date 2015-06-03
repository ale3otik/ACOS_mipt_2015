#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>

#define N 100

const int P = 100;

static pthread_mutex_t mutex;
static pthread_cond_t cond;

/* Здесть всё плохо:
 *
 *
 * Variably modified array at file scope
 *
 *
 * The reason for this warning is that const in c 
 * doesn't mean constant. It means "read only". 
 * So the value is stored at a memory address and 
 * could potentially be changed by machine code.
 */
static int matrix[N][N];

int count_sync = 0;
int count_game = 0;
int game_is_exit = 0;
int count_jump = 0;

void * function(void * my_pid)
{
	int count_lucky = 0;
	int i = 0; 
	int j = 0;
	srand(time(NULL));
	while(1)
	{
			if(game_is_exit) break;

			i = rand() % N;
			j = rand() % N;
			pthread_mutex_lock(&mutex);

			if(count_jump == 10)
			{
					count_sync++;
					
					if(count_sync != P)
					{
						pthread_cond_wait(&cond,&mutex);
						if(game_is_exit == 1)
						{
							pthread_mutex_unlock(&mutex);
							break;
						}
					}
					else
					{
						count_jump = 0;
						count_sync = 0;
						count_game++;
						if(count_game == 10)
						{
							game_is_exit = 1;
						}
						pthread_cond_broadcast(&cond);
						if(game_is_exit == 1)
						{
							pthread_mutex_unlock(&mutex);
							break;
						}
					}
			}

			if(matrix[i][j] == 0)
			{
				matrix[i][j] = getpid();
				count_lucky++;
				count_jump++;

				pthread_mutex_unlock(&mutex);
			}
			else
			{
				pthread_mutex_unlock(&mutex);
				sleep(rand()%60000 + 1);
			}
	}
	return NULL;
}

int main()
{

	return 0;
}
