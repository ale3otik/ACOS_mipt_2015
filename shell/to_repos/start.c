#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include "my_types.h"
#include <sys/types.h>
#include <pwd.h>
#include <uuid/uuid.h>

program * start_program = NULL;
pid_t main_pid = -1;
volatile static short is_processes_killed_by_signal = 0;
void hand_signal(int sig)
{
	program * cur_prog = start_program;

	if(start_program != NULL)
	{
		while(cur_prog != NULL)
		{
			kill(cur_prog->pid, sig);
			cur_prog = cur_prog->output_programm_ptr;
		}
		is_processes_killed_by_signal = 1;
		kill(main_pid,SIGCHLD);
		return;
	}
	else
	{
		exit(1);
	}
}


int do_next_action(string * command_str);
void create_pipe_sequence(int tasks_quantity);
int main(int argc, char ** argv , char ** env)
{
	int check_state = 0;
	string command_str;
	shell_env_params shell_params;
	struct passwd * user_info;
	user_info = getpwuid(getuid());

	shell_params.shell_pid = getpid();
	shell_params.uid = user_info->pw_uid;
	shell_params.argc = argc;
	shell_params.argv = argv;
	shell_params.last_job_state = INITIALIZED_VALUE;
	shell_params.shell_name = argv[0];
	shell_params.user_name = user_info->pw_name;
	shell_params.home_path = user_info->pw_dir;
	shell_params.current_path = env[11];
	command_str.data = NULL;

	main_pid = getpid();
	signal(SIGINT,hand_signal);
	signal(SIGTSTP,hand_signal);
       /* printf("%s\n", shell_params.current_path);*/
	while(1)
	{
		long offset = 0;
		int check;
		printf("%s$ ", shell_params.user_name);
		string_construct(&command_str);
		if(!string_get(stdin,&command_str)) /*read command*/
        { 
            check_state = EXIT;
            break;
        } 
        offset = string_get_next_position(&command_str,offset);
        if(command_str.data[offset] == 0 || command_str.data[offset] == '#' ) continue;	
        
        check = do_next_action(&command_str);
        if(check == EXIT) break;
	}
	string_delete(&command_str);
	return 0;
}	


int do_next_action(string * command_str)
{
	long offset = 0;
	program * last_program = NULL;
	program * cur_program = NULL;
	int count_tasks = 0;
	while(1) /*get list of programms to pipe-sequence*/
	{
		cur_program = (program * )malloc(sizeof(program));
			errcheck(cur_program, NULL);
		if(start_program == NULL) start_program = cur_program;
	
		get_program_info(cur_program,command_str,&offset);
		
		if(cur_program->name != NULL)
		{
			++count_tasks;
		}
		else
		{
			program_destroy(cur_program);
			free(cur_program);
			if(start_program == cur_program) start_program = NULL;
			cur_program = NULL;

			break;
		}

		if(last_program != NULL)
		{
			last_program->output_programm_ptr = cur_program;
		}

		if(command_str->data[offset] == '|') ++offset;
		else break;

		last_program = cur_program;
	}
	if(start_program != NULL 
		&&  strlen(start_program->name) == strlen("exit")
		&&  strncmp(start_program->name,"exit",strlen("exit"))== 0 )
			return EXIT;

	create_pipe_sequence(count_tasks);
	return NORMAL;
}

void create_pipe_sequence(int tasks_quantity)
{
	fileds_t * pipe_fileds;
	int i;
	program * last_program = NULL;
	program * cur_program = start_program;

	pipe_fileds = (fileds_t * )malloc(sizeof(fileds_t) * (tasks_quantity - 1));
		errcheck(pipe_fileds, NULL);

	for(i = 0; i < tasks_quantity-1; ++i)
	{
		pipe(pipe_fileds[i].id);
	}

	for(i = 0; i < tasks_quantity; ++i)
	{
		if((cur_program->pid = fork()) == 0)
		{
			int id = 0;
			if(i == 0 || i == tasks_quantity-1)
			{
				if(cur_program->input_file!= NULL)
				{
					id = open(cur_program->input_file,O_RDONLY);
					fprintf(stderr, " \n :in:%s::", cur_program->input_file);
					if(id == -1)
					{
						errprint("can't open input file");
					}
					else 
					{
						if(dup2(id,0) == -1) errhand("cant dup2");
						errprint("succes INPUT duplicate");
					}
				}
				if(cur_program->output_file!= NULL)
				{
					fprintf(stderr, " \n :out:%s::", cur_program->output_file);
					if(cur_program->output_type == REWRITE)
					{
						creat(cur_program->output_file,0666);
						id = open(cur_program->output_file,O_WRONLY);
						fprintf(stderr, " \n TRY CREATE");
					}
					else
					{
						id = open(cur_program->output_file,O_APPEND);
						fprintf(stderr, " \n TRY OPEN");
					}
	
					if(id == -1)
					{
						errprint("can't open output file");
					}
					else 
					{
						if(dup2(id,1) == -1) errhand("cant dup2");
						errprint("succes OUT duplicate");
					}
				}
			}

			if(i > 0) dup2(pipe_fileds[i].id[0],0);
			if(i < tasks_quantity - 1) dup2(pipe_fileds[i+1].id[1],1);
			errcheck(cur_program , NULL);
			
			fprintf(stderr, " >%d<\n", id);
			if(execvp(cur_program->name,cur_program->arguments) == -1){
				errhand("can't run this program!");
			}
		}

		last_program = cur_program;
		cur_program = cur_program->output_programm_ptr;
	}

	for(i = 0; i < tasks_quantity-1; ++i)
	{
		close(pipe_fileds[i].id[0]);
		close(pipe_fileds[i].id[1]);
	}

	for(i = 0; i < tasks_quantity; ++i) 
	{
		if(is_processes_killed_by_signal)
		{
			is_processes_killed_by_signal = 0;
			break;
		}
		wait(NULL);
	}
	free(pipe_fileds);
	
	cur_program = start_program;
	start_program = NULL;
	for(i = 0; i < tasks_quantity && cur_program!= NULL; ++i) 
	{
		program * tmp = cur_program;
		cur_program = cur_program->output_programm_ptr;
		program_destroy(tmp);
		free(tmp);
	}
}
