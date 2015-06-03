
#ifndef _MY_TYPES_H_
#define _MY_TYPES_H_
#include <unistd.h>

#define TRUE 1
#define FALSE 0

#define MID_COMMAND_LENGTH 50
#define MAX_NAME_LENGTH 150
#define MAX_NUMBER_OF_ARGS 30
#define EXIT -666
#define NORMAL 0
#define INITIALIZED_VALUE 253
#define APPEND 1
#define REWRITE 2

#define errhand(n) {fprintf(stderr,"\n%s line: %d ", n , __LINE__); exit(1);}
#define errcheck(v , check_v){if( v == check_v ) errhand("errcheck");}
#define errprint(n) {fprintf(stderr,"\n ERROR: %s in line %d \n", n , __LINE__);}


struct shell_env_params
{
	int argc;
	char ** argv;
	int last_job_state;
	char * user_name;
	char * home_path;
	char * shell_name;
	uid_t uid;
	char * current_path;
	pid_t shell_pid;
};
typedef struct shell_env_params shell_env_params;

struct program
{
	char * name ;
	int number_of_arguments;
	char ** arguments ;
	char * input_file; 
	char * output_file;
	short output_type;
	struct program * output_programm_ptr;
	pid_t pid;
};
typedef struct program program;

struct fileds_t 
{
	int id[2];
};
typedef struct fileds_t fileds_t;

#include "strings.h"
#include "functions.h"

#endif
