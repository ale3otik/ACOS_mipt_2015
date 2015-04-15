
#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_
#include "my_types.h"
void smart_get_filename(char * file_name,string * command_str,size_t * offset);
void smart_get_string(string * dest,string * command_str,size_t * offset,int param); /*param - stop on '\n' or not*/

int parse_key(char * str, params_of_openfile * params); 
void initialization_params(params_of_openfile *);

size_t string_get_next_position(struct string * str,size_t offset);
void string_get(FILE * file_input,string * _str);

long string_get_number(string * command,size_t * offset);

char * get_next_word(string * str, size_t * offset);
/**************************************************/
long cartesian_size(struct cartesian_tree * _tree);

void insert_after_special_function(string* command,size_t offset,data_container ** data, long position);

int special_edit_insert_function(string * command,
								  size_t offset, 
								  data_container ** data,
								  long array_position,
								  long string_position);
#endif
