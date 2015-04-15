#include "my_types.h"
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

void smart_get_filename(char * file_name,string * command_str,size_t * offset)
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

void smart_get_string(string * dest,string * command_str,size_t * offset, int param)
{
	*offset = string_get_next_position(command_str, *offset);
	while(1)
	{
		if(command_str->data[*offset] == '\"' || command_str->data[*offset] == 0) return;

		if(command_str->data[*offset] == '\\' )
		{
			++*offset;
			if(command_str->data[*offset] == '\0' ) return;
			if(command_str->data[*offset] == 'n') 
			{
				if(param == ALL_STRING)
				{
					string_push_back(dest, '\n');
					++*offset;
					continue;
				}
				else
				{
					return; /* new string*/
				}
			}
		}
		string_push_back(dest, command_str->data[*offset]);
		++ *offset;
	}
}

long string_get_number(string * command,size_t * offset)
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
/********************************************************************************/

void initialization_params(params_of_openfile * params)
{
    params->binary_file = FALSE;
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


void insert_after_special_function(string* command,size_t offset,data_container ** data, long position)
{
	long count_strings = 0;
  	offset = string_get_next_position(command,offset);
    if(command->data[offset] != '\"')
    {
        fprintf(stderr,"\nNo strings added. Use \"YOUR_ADDED_STR\" to insert str");
        return;
    }

    string  inserting_string;
    inserting_string.data = NULL;
    string_construct(&inserting_string);

    ++offset;
    if(command->data[offset] == '\"')
    {
        ++offset;
        if(command->data[offset] == '\"') /* insert several strings*/
        {
            ++offset;
            while(1)
            {
                smart_get_string(&inserting_string, command, &offset,!ALL_STRING);
                if(command->data[offset] == '\0')
                {
                    cart_tree_insert(data,&inserting_string,position);

                    string_construct(command);
                    string_construct(&inserting_string);
                    
                    offset = 0;
                    ++count_strings;
                    ++position;
                    fprintf(stdout,"\nstr %ld: ",count_strings+1);
                    string_get(stdin,command); /*read command*/

                    continue;
                }
                else
                {
                    if(command->data[offset] == '\"')
                    {
                        ++offset;
                        if(command->data[offset] == '\"')
                        {
                            ++offset;
                            if(command->data[offset] == '\"')
                            {
                                cart_tree_insert(data,&inserting_string,position);
                                break;
                            }
                            string_push_back(&inserting_string,'\"');
                        }
                        string_push_back(&inserting_string,'\"');
                    }
                }
            }
            string_delete(&inserting_string);

            fprintf(stderr,"\ninserted %ld succes",count_strings+1);
            return;
        }
        --offset;
    }

    while(1)
    {
        smart_get_string(&inserting_string, command,&offset,!ALL_STRING);
        cart_tree_insert(data, &inserting_string, position);
        if(command->data[offset] == 'n')
        {       
            ++position;
            string_construct(&inserting_string);
            ++offset;
            ++count_strings;

            continue;
        }
        string_delete(&inserting_string);

        fprintf(stderr,"\ninserted %ld succes",count_strings+1);
        return;
    }
}

/* isert several strings inside array_position string before atring_position elem*/
int special_edit_insert_function(string * command,
								  size_t offset, 
								  data_container ** data,
								  long array_position,
								  long string_position)
{
	string * finding_string;
	int count_strings = 0;
	cart_tree_access(data, array_position, &finding_string);
    if(string_position <  0)
    {
        string_position = finding_string->size;
    }

	if(string_position > finding_string->size)
        {
            fprintf(stderr,"\nERROR: wrong asnwer %ld ",string_position);
        }
        int index = 0; /*counter*/
        if(command->data[offset++] != '\"')
        {
            fprintf(stderr,"\nERROR: use \"YOUR_INSERTING_STRING\"");
            return 0;
        }

        while(command->data[offset] != '\0' && command->data[offset] != '\"')
        {
            string inserting_string;

            inserting_string.data = NULL;
            string_construct(&inserting_string);
            smart_get_string(&inserting_string, command, &offset,!ALL_STRING);
            index = 0;
            if(command->data[offset] == 'n') 
            {
                string_push_back(&inserting_string, '\n');
                ++offset;
            }
            while(inserting_string.data[index] != 0)
            {
                string_insert(finding_string, inserting_string.data[index],string_position);
                ++index;
                ++string_position;
            }

            string_delete(&inserting_string);
        }

        if(command->data[offset] == '\0')
        {
            fprintf(stderr,"\nERROR: use \"YOUR_INSERTING_STRING\"");
            return 0;
        }

        string string_to_push_in_arr;
        string_to_push_in_arr.data = NULL;
        string_construct(&string_to_push_in_arr);

        for(index = 0;index < finding_string->size;++index)
        {
            if(finding_string->data[index] == '\n')
            {
                /*fprintf(stderr,"<%ld>\n",cartesian_size(*data));*/
                cart_tree_insert(data, &string_to_push_in_arr, array_position);
                ++array_position;
                string_construct(&string_to_push_in_arr);
                ++count_strings;
            }
            else
            {
                string_push_back(&string_to_push_in_arr,finding_string->data[index]);
            }

        }

        cart_tree_insert(data, &string_to_push_in_arr, array_position);
        ++count_strings;
        array_position += 2; /* magic value*/
        cart_tree_remove(data, array_position, array_position,TRUE);
        string_delete(&string_to_push_in_arr);
        return count_strings;
}
