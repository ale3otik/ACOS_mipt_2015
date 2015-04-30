#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/signal.h>


#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>

void action_signal(int signum, siginfo_t * info, void * context) 
{
	printf("\nI GET SIGNAL FROM: %d\n NAME: \"%s\"\n",info->si_pid , getpwuid(info->si_uid)->pw_name);

	exit(1);
}

void set_all_signal(struct sigaction * action)
{
	int i;
	for(i =0;i<32;++i)
	{
		sigaction(i,action,NULL);
	}
}
int main()
{		
	struct sigaction action;

	printf("\n My pid is %d\n",getpid());

	action.__sigaction_u.__sa_sigaction = action_signal;
	action.sa_mask = 0;
	action.sa_flags = 0;

	set_all_signal(&action);

	while(1)
	{
		sleep(100000); 
	}

	return 0;
}