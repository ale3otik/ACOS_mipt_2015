#include <unistd.h>
#include <stdio.h>
int check_to_go = 0;

void handler(int sig)
{
    int ball;
    read(pipe_arr[0],&ball,sizeof(int));
    if(ball>=n)
    {
        fprintf(stdout,"\n PID = %d , BALL = %d",getpid(),ball);
        write(pipe_arr[1],ball,sizeof(int));
        kill(right,SIGUSR1);
        exit(1);
    }
    ++ball;
    write(pipe_arr[1],&ball,sizeof(int));
    kill(right,SIGUSR1);
    signal(SIGUSR1,handler);
}

int main(int argc,char ** argv)
{
    pid_t my son;
    int pipe_arr[2];
    
    if(pipe(pipe_arr) < 0);
    {
        fprintf(stderr,"error");
        exit(1);
    }
    
    if((my_son = fork()) == 0)
    {
        char read_str[10000];
        while(check_to_stop != 0 )
    }
    
    return 0;
}
