
#ifndef MYTYPES_H_INCLUDED
#define MYTYPES_H_INCLUDED

#define ERROR -1
#define SUCCES 1

#define TRUE 1
#define FALSE 0

#define STANDART_INPUT_ID 0 

typedef struct winsize winsize;


typedef struct string string;


typedef struct cartesian_tree data_container; /* the main container of strings*/


#define MAX_NAME_LENGTH 300
#define MID_COMMAND_LENGTH 100
#define MAX_STRING_LENGTH 100

#define START_STRING_SIZE 32
	
/* COMANDS CODE*/
#define PRINT_PAGES 121
#define PRINT_RANGE 122
#define INSERT_AFTER 123
#define EDIT_STRING 124
#define REPLACE 125
#define DELETE_RANGE 126
#define DELETE_BRACES 127

#define READ 128
#define OPEN 129
#define WRITE 130
#define EXIT 131
#define HELP 132
#define NO_ACTIONS_NEED 133
#define ALL_STRING 1 

#define NORMAL_REPLACE_MODE 1
#define BEGIN_REPLACE_MODE 2 /* replace with the "^" command*/
#define END_REPLACE_MODE 3 /*replace with the "$" command*/


#define F_BOTH 1
#define F_TO_READ 2
#define F_TO_WRITE 3

#define errprint(str) fprintf(stderr, str )
#endif 

#include "functions.h"
#include "file_operations.h"
#include "strings.h"
#include "cartesian_tree.h"
#include "editor.h"

