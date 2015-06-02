#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

pid_t get_grandson_pid[2] = {-1,-1};
pid_t brothers[2] = {-1,-1};

void action_signal(int signum, siginfo_t * info, void * context) 
{
	static int index = 0;
	get_grandson_pid[index] = info->si_pid;
	++index;
}

/*void action_signal_get_brother(int signum, siginfo_t * info, void * context) 
{
	static int index = 0;
	brothers[index] = info->si_pid;
	++index;
}
*/
int main()
{
	pid_t father = getpid();
	pid_t son[2];
	int i;
	int pipe_fd[2];
	struct sigaction action;
	action.__sigaction_u.__sa_sigaction = action_signal;
	action.sa_mask = 0;
	action.sa_flags = 0;
	sigaction(SIGUSR1 , &action , NULL); 

	pipe(pipe_fd);

	for(i = 0; i < 2; ++i)
	{
		if((son[i] = fork()) == 0) 
		{
			pid_t grand_son;
			pid_t i_m_son = getpid();
			/*struct sigaction get_brother;
			action.__sigaction_u.__sa_sigaction = action_signal_get_brother;
			action.sa_mask = 0;
			action.sa_flags = 0;
			sigaction(SIGUSR1 , &get_brother,NULL); 
			*/

			if((grand_son = fork()) == 0)
			{
					printf("%d %d %d", father, i_m_son, getpid());			

					kill(father,SIGUSR1);
					exit(1);
			}
			while(read(pipe_fd[0], son, 2 * sizeof(pid_t)) <= 0);
			
			pid_t my_brother_pid = son[0];
			if(my_brother_pid == getpid()) my_brother_pid = son[1];

			printf(" %d %d %d", getpid(),father,my_brother_pid);

			wait(NULL);
			exit(1);
		}

	}

	write(pipe_fd[1],son,2*sizeof(pid_t));
	
	wait(NULL);
	wait(NULL);
	if(get_grandson_pid[0] == -1 || get_grandson_pid[1]) pause();
	if(get_grandson_pid[0] == -1 || get_grandson_pid[1]) pause();

	printf("%d %d" , get_grandson_pid[0], get_grandson_pid[1]);


	return 0;
}