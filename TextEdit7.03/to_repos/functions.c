#include "my_types.h"
#include <ctype.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>

int min(int a,int b)
{
    if(a<b) return a;
    return b;
}


long string_get_next_position(string * str,long offset)
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


void smart_get_string(string * dest,string * command_str,long * offset, int param)
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


long string_get_number(string * command,long * offset)
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


char * get_next_word(string * str, long * offset)
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


void insert_after_special_function(string* command,long offset,data_container ** data, long position)
{
	long count_strings = 0;
    string inserting_string;

  	offset = string_get_next_position(command,offset);
    if(command->data[offset] != '\"')
    {
        fprintf(stderr,"\nNo strings added. Use \"YOUR_ADDED_STR\" to insert str");
        return;
    }

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
                    fprintf(stderr,"\nstr %ld: ",count_strings+1); 
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
								  long offset, 
								  data_container ** data,
								  long array_position,
								  long string_position)
{
	string * finding_string;
    string string_push_in_arr;
	int count_strings = 0;
    int index = 0; /*counter*/

	cart_tree_access(data, array_position, &finding_string);
    if(string_position <  0)
    {
        string_position = finding_string->size;
    }

	if(string_position > finding_string->size)
    {
        fprintf(stderr,"\nERROR: wrong asnwer %ld ",string_position);
    }
        
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

        string_push_in_arr.data = NULL;
        string_construct(&string_push_in_arr);

        for(index = 0;index < finding_string->size;++index)
        {
            if(finding_string->data[index] == '\n')
            {
                /*fprintf(stderr,"<%ld>\n",cartesian_size(*data));*/
                cart_tree_insert(data, &string_push_in_arr, array_position);
                ++array_position;
                string_construct(&string_push_in_arr);
                ++count_strings;
            }
            else
            {
                string_push_back(&string_push_in_arr,finding_string->data[index]);
            }

        }

        cart_tree_insert(data, &string_push_in_arr, array_position);
        ++count_strings;
        array_position += 2; /* magic value*/
        cart_tree_remove(data, array_position, array_position,TRUE);
        string_delete(&string_push_in_arr);
        return count_strings;
}


long get_ranges(long * range,string * command_str, cartesian_tree * data, long offset)
{
    range[0] = -1;
    range[1] = -1;
    offset = string_get_next_position(command_str,offset);

    range[0] = string_get_number(command_str,&offset);
    offset = string_get_next_position(command_str,offset);
    range[1] = string_get_number(command_str, &offset);

    if(range[0] == -1) range[0] = 1;
    if(range[1] == -1)
    {
        range[1] = cartesian_size(data);
    }
    --range[0];
    --range[1];

    if(range[0] > range[1] || range[0] <0 || range[1] <0 || range[1] >= cartesian_size(data))
    {
        fprintf(stderr,"\nERROR: wrong range %ld %ld",range[0]+1,range[1]+1);
        return -1;
    }
   /* ++offset;*/
    return offset;
}


/***************special functions to delete braces command*********************/
int string_scan_braces(int * braces_deep_level, string * scan_str)
{
    short is_error = FALSE;
    char * scan_ptr = scan_str->data;

    while(*scan_ptr != '\0')
    {
        if(*scan_ptr == '{')
            ++*braces_deep_level;
            
        if(*scan_ptr == '}' && --*braces_deep_level < 0)
        {
            is_error = TRUE;
            break;
        }

        ++scan_ptr;
    }
    return is_error;
}

void string_delete_braces(int * braces_deep_level, string* source_str)
{
    char * del_begin = source_str->data;
    long del_size = 0;
    char * scan_ptr = source_str->data;

    while(*scan_ptr != '\0')
    { 
        if(*scan_ptr == '{')
        {
            if(!*braces_deep_level)
            {
                del_begin = scan_ptr;
                del_size = 0;
            }

            ++*braces_deep_level;
        }

        if(braces_deep_level)
            ++del_size;
       
        if(*scan_ptr == '}')
        {
            --*braces_deep_level;

            if(!*braces_deep_level)
            {
                string_replace(source_str, del_size, NULL, (long)(del_begin - source_str->data));
                scan_ptr -= del_size;
                del_size = 0;
            }
        }

        ++scan_ptr;
    } 

    if(*braces_deep_level)
        string_replace(source_str, del_size, NULL, (long)(del_begin - source_str->data));
}

void tree_delete_braces(cartesian_tree** tree_ptr)
{
    int braces_deep_level = 0;
    short is_error = FALSE;
    string* cur_str;
    cartesian_tree* tree = *tree_ptr;
    cartesian_tree* left_tree = NULL;
    cartesian_tree* right_tree = NULL;
    cartesian_tree* list = NULL;
    cartesian_tree* end_list = NULL;
    cartesian_tree* list_ptr;

    conv_tree_to_list(&list, &end_list, &tree);

    list_ptr = list;

    while(TRUE)
    {
        is_error = string_scan_braces(&braces_deep_level, list_ptr->text);
        if(is_error)
        {
            fprintf(stderr, "Can't do this operation: braces aren't balanced.\n");
            break;
        }

        if(list_ptr == end_list)
        {
            if(braces_deep_level > 0)
            {   
                is_error = -1;
                fprintf(stderr, "Can't do this operation: braces aren't balanced.\n");
            }
            break;
        }

        list_ptr = list_ptr->right;
    }

    if(!is_error)
    {
        list_ptr = list;

        while(TRUE)
        {
            string_delete_braces(&braces_deep_level,list_ptr->text);

            cur_str = list_ptr->text;
            if(*cur_str->data == '\0')
            {
                left_tree = list_ptr->left;
                right_tree = list_ptr->right;

                list_ptr->left = NULL;
                list_ptr->right = NULL;
                tree_delete(list_ptr);

                if(left_tree)
                    left_tree->right = right_tree;
                else
                    list = right_tree;

                if(right_tree)
                    right_tree->left = left_tree;
                else
                {
                    end_list = left_tree;
                    break;
                }

                list_ptr = right_tree;
            }
            else
            {
                if(list_ptr == end_list)
                    break;

                list_ptr = list_ptr->right;
            }
        }
    }

    conv_list_to_tree(&list, &end_list, &tree);   
    *tree_ptr = tree; 
}

/**********************************************/

int count_num_symbols(long num)
{
    int count = 0;

    if(num < 0) num *= -1;
    do
    {
        num/= 10;
        ++count;
    }while(num > 0);

    return count;
}


int super_print_string(string * printed_string, winsize * size_of_term,long * num,int * balance) /*TRUE if need new str*/
{
    static unsigned long next_printed_index = 0;
    int count_printed_symbols = 0;
    int count_printed_strings = 0;

    if(next_printed_index == 0)
    {
        count_printed_symbols = 1 + count_num_symbols(*num) + printed_string->size;
    }
    else
    {
        count_printed_symbols = strlen(printed_string->data + next_printed_index);
    }

    count_printed_strings = (count_printed_symbols+size_of_term->ws_col)/size_of_term->ws_col; 
    if(*balance - count_printed_strings >= 0)
    {
        if(*balance != size_of_term->ws_row) 
        {
            fprintf(stdout,"\n");
        }
        if(next_printed_index == 0)
        {
            fprintf(stdout,"%ld>",*num);
        }

        fprintf(stdout,"%s",printed_string->data + next_printed_index);
        ++ *num;
        *balance -= count_printed_strings;
        next_printed_index = 0;
        return TRUE;
    }
    else
    {
        count_printed_strings = *balance;
        count_printed_symbols  =count_printed_strings * size_of_term->ws_col;
        
        if(*balance != size_of_term->ws_row) 
        {
            fprintf(stdout,"\n");
        }
        if(next_printed_index == 0)
        {
            fprintf(stdout,"%ld>",*num);
            count_printed_symbols += 1 + count_num_symbols(*num);
        }
        fwrite(printed_string->data + next_printed_index, sizeof(char), count_printed_symbols,stdout);

        *balance = 0;
        next_printed_index += count_printed_symbols;
        return FALSE;
    }
}

