
#include "functions.h"
#include <stdlib.h>
#include <string.h>
long string_get_next_position(string * str,long offset)
{
	if(str == NULL)
	{
		fprintf(stdout, "ERROR: nullptr string in func: string_get_next_position" );
		exit(1);
	}
    while(str->data[offset] != '\0' && (str->data[offset] == ' ' || str->data[offset] == '\t'))
    {
        ++offset;
    }
    return offset;
}


char * get_next_word(string * str, long * offset)
{
    int index = 0;

    char * word = (char*) malloc(sizeof(char) * (MAX_NAME_LENGTH+1));
    if(word == NULL)
    {
        fprintf(stdout,"\nERROR: malloc in get_next_word %d ",MAX_NAME_LENGTH+1);
        exit(1);
    }

    for(;str->data[*offset] != '\0' &&
        str->data[*offset] != ' ' &&
        str->data[*offset] != '\t' &&
        str->data[*offset] != '#' && 
        *offset < MAX_NAME_LENGTH;++*offset,++index)
    {
        word[index] = str->data[*offset];
    }
    word[index] = '\0';
    
    return word;
}

int string_get(FILE * file_input,string * _str)
{
    char buffer[MID_COMMAND_LENGTH];
    int is_first = TRUE;
    do
    {
        fgets(buffer,MID_COMMAND_LENGTH-1,file_input);
        if(feof(file_input)) 
        {
            if(is_first) return FALSE;
            break; 
        }

        string_big_push(_str,buffer,strlen(buffer));
        is_first = FALSE;
    }while(buffer[strlen(buffer)-1] != '\n' && !feof(file_input));  
    return TRUE;
}

void program_construct(program * prog)
{
    if(prog == NULL) return;
    prog->output_programm_ptr = NULL;
    prog->arguments = (char **) malloc(sizeof(char) * MAX_NUMBER_OF_ARGS);
    prog->number_of_arguments = 0;

    prog->name = (char *)malloc(sizeof(char) * MAX_NAME_LENGTH);
        errcheck(prog->name,NULL);
    prog->name[0] = 0;
    
    prog->input_file = NULL;
    prog->output_file = NULL;
    prog->pid = -1;

    prog->output_type = NORMAL;
}
void program_destroy(program * prog)
{
    int i;
    if(prog == NULL) return;
    free(prog->name);
    for(i = 1; i < prog->number_of_arguments; ++i)
    {
        free(prog->arguments[i]);
    }
    free(prog->arguments);
    free(prog->input_file);
    free(prog->output_file);

    prog->pid = -1;
    prog->name = NULL;
    prog->input_file = NULL;
    prog->output_file = NULL;
    prog->arguments = NULL;
    prog->output_type = NORMAL;
    prog->number_of_arguments = 0;
}


void smart_get_filename(char * file_name,string * command_str,long * offset)
{
    int index;

    *offset = string_get_next_position(command_str, *offset);
    for (index = 0; index < MAX_NAME_LENGTH-1; ++index,++*offset)
    {
        if(command_str->data[*offset] == '#') break; /* next is comment*/
        if(command_str->data[*offset] == '\0' || command_str->data[*offset] == ' ' || command_str->data[*offset]=='\t') break;
        if(command_str->data[*offset] == '\\')
        {
            ++*offset;
            /*if(command_str->data[*offset] == 'n')
            {
                file_name[index] = '\n';
                continue;
            }
            */
            if(command_str->data[*offset] == 't')
            {
                file_name[index] = '\t';
                continue;
            }
            else if(command_str->data[*offset] == ' ')
            {
                file_name[index] = ' ';
                continue;
            }
            else if(command_str->data[*offset] == '#')
            {
                file_name[index] = '#';
                continue;
            }
            else if(command_str->data[*offset] == '\\')
            {
                file_name[index] = '\\';
                continue;
            }
        }
        file_name[index] = command_str->data[*offset];
    }
    file_name[index] = '\0';
    *offset = string_get_next_position(command_str, *offset);
}


void get_program_info(program * prog, string * command, long * offset)
{
    int arg_quantity = 0;
    program_construct(prog);

    smart_get_filename(prog->name,command,  offset);
    if(prog->name == 0)
    {
        program_destroy(prog);
        return;
    }
    prog->arguments[arg_quantity] = prog->name;
    ++arg_quantity;

    while(1) /*args*/
    {
        char * next_arg = (char *) malloc(sizeof(char) * MAX_NAME_LENGTH);
        if(command->data[*offset] == 0 
            || command->data[*offset] == '>' 
            || command->data[*offset] == '<'
            || command->data[*offset] == '|'
            || command->data[*offset] == '#')
        {
            free(next_arg);
            break;
        }   


        smart_get_filename(next_arg,command,offset);
        if(arg_quantity < MAX_NUMBER_OF_ARGS-1)
        {
            prog->arguments[arg_quantity] = next_arg;
            ++arg_quantity;
        }
        else
        {
            free(next_arg);
        }
    }
    prog->arguments[arg_quantity] = 0;
    ++arg_quantity;
    prog->number_of_arguments = arg_quantity;

    while(1)/*files*/
    {
        char * file_name = (char * ) malloc(sizeof(char) * MAX_NAME_LENGTH);
        short state = NORMAL;
        if(command->data[*offset] == 0 || command->data[*offset] == '|' || command->data[*offset] == '#')
        {
            break;
        }        
        if(command->data[*offset] == '>')
        {
            ++*offset;
            if(command->data[*offset] == '>')
            {
                ++*offset;
                state = APPEND;
            }
            else
            {
                state = REWRITE;
            }
        }
        else if(command->data[*offset] == '<')
        {
            ++ *offset;
            state = 3;
        }
        
        smart_get_filename(file_name,command, offset);
        if(state > 0)
        {
            if(state == 3 && prog->input_file == NULL)
            {
                prog->input_file = file_name;
                continue;
            }
            if(state < 3 && prog->output_file == NULL)
            {
                prog->output_file = file_name;
                prog->output_type = state;   
                continue;
            }
        }
        free(file_name);
    }
}

