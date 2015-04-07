#include "MyTypes.h"
#include <ctype.h>
#include <string.h>

size_t string_get_next_position(string * str,size_t offset)
{
	if(str == NULL)
	{
		fprintf(stderr, "ERROR: nullptr string in func: string_get_next_position" );
		exit(1);
	}
    while(str->data[offset] != '\0' && (str->data[offset] == ' ' || str->data[offset] == '\t'))
    {
        ++offset;
    }
    return offset;
}

void string_get(FILE * file_input,string * _str)
{
    void * error_check;
    char buffer[MID_COMMAND_LENGTH];
      do
        {
            error_check = fgets(buffer,MID_COMMAND_LENGTH-1,file_input);
            if(feof(file_input)) break;
            if(error_check == NULL)
            {
                fprintf(stderr,"\nSCAN ERROR(tring_get)\n");
                exit(1);
            }
            string_big_push(_str,buffer,strlen(buffer));
        }while(buffer[strlen(buffer)-1] != '\n' && !feof(file_input));     
}

void smart_get_line(char * file_name,string * command_str,size_t * offset)
{
	int index;

	*offset = string_get_next_position(command_str, *offset);
	for (index = 0; index < MAX_NAME_LENGTH-1; ++index,++*offset)
	{
		if(command_str->data[*offset] == '\0' || command_str->data[*offset] == ' ' || command_str->data[*offset]=='\t') break;
		if(command_str->data[*offset] == '\\')
		{
			++*offset;
			if(command_str->data[*offset] == 'n')
			{
				file_name[index] = '\n';
				continue;
			}

			if(command_str->data[*offset] == 't')
			{
				file_name[index] = '\t';
				continue;
			}
		}
		file_name[index] = command_str->data[*offset];
	}
    file_name[index] = '\0';
}

void initialization_params(params_of_openfile * params)
{
    params->binary_file = FALSE;
}

 int string_get_number(string * command,size_t * offset)
 {
 	if(!isdigit(command->data[*offset]))
 	{
 		return -1;
 	}
 	else
 	{
 		int number = 0;
 		for(;isdigit(command->data[*offset]);++ *offset)
 		{
 			number *= 10;
 			number += command->data[*offset] - '0';
 		}
 		return number;

 	}
 }
int parse_key(char * str , params_of_openfile * params)
{   
    size_t ofset = 0;

    if(strncmp(str,"-bin",MAX_NAME_LENGTH) == 0 ||
       strncmp(str,"-b",MAX_NAME_LENGTH) == 0 ||
       strncmp(str,"-binary",MAX_NAME_LENGTH) == 0 )
    {
        params->binary_file = 1;
    }
    return ofset;
}

char * get_next_word(string * str, size_t * offset)
{
	int index = 0;

	char * word = (char*) malloc(sizeof(char) * (MAX_NAME_LENGTH+1));
	if(word == NULL)
	{
		fprintf(stderr,"\nERROR: malloc in get_next_word %d ",MAX_NAME_LENGTH+1);
		exit(1);
	}

	for(;str->data[*offset] != '\0' &&
	 	str->data[*offset] != ' ' &&
	  	str->data[*offset] != '\t' && 
	  	*offset < MAX_NAME_LENGTH;++*offset,++index)
	{
		word[index] = str->data[*offset];
	}
	word[index] = '\0';
	
	return word;
}


