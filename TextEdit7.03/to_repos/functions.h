
#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "my_types.h"

int min(int a,int b);

void smart_get_filename(char * file_name,string * command_str,long * offset); /*to read file name from string to char[] */
void smart_get_string(string * dest,string * command_str,long * offset,int param); /*param - stop on '\n' or not*/


long string_get_next_position(struct string * str,long offset); /* to ignore spaces*/
int string_get(FILE * file_input,string * _str); /* to read string sny size from file*/


char * get_next_word(string * str, long * offset); /* to copy next word from sting to char[] */

int string_get_number(string * command,long * offset, long * number);

/*used by insert_after command*/
void insert_after_special_function(string* command,long offset,data_container ** data, long position); 

/*used by edit string insert command function*/
int special_edit_insert_function(string * command,
								  long offset, 
								  data_container ** data,
								  long array_position,
								  long string_position);

/*to get area of apply some function (or just get 2/1/0 numbers from string,started from offset position)*/
long get_ranges(long * range ,string * command_str, cartesian_tree * data, long offset);

/*used by delete_braces command*/
void tree_delete_braces(cartesian_tree** tree_ptr);
/* used by print_pages, this function remember last printed position in last string and can continue*/
int super_print_string(string * printed_string, int col_size_of_term,int row_size_of_term,long * num,int * balance);
#endif
