
#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "my_types.h"
void smart_get_filename(char * file_name,string * command_str,long * offset);
void smart_get_string(string * dest,string * command_str,long * offset,int param); /*param - stop on '\n' or not*/

long parse_key(char * str, params_of_openfile * params);
void initialization_params(params_of_openfile *);

long string_get_next_position(struct string * str,long offset);
int string_get(FILE * file_input,string * _str);

long string_get_number(string * command,long * offset);

char * get_next_word(string * str, long * offset);
/**************************************************/
long cartesian_size(struct cartesian_tree * _tree);

void insert_after_special_function(string* command,long offset,data_container ** data, long position);

int special_edit_insert_function(string * command,
								  long offset, 
								  data_container ** data,
								  long array_position,
								  long string_position);

long get_ranges(long * range ,string * command_str, cartesian_tree * data, long offset);

void in_order_delete_braces(cartesian_tree** tree_ptr);
int super_print_string(string * printed_string, winsize * size_of_term,long * num,int * balance);
#endif
