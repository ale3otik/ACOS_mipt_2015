
#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_
#include "MyTypes.h"
void smart_get_filename(char * file_name,string * command_str,size_t * offset);
void smart_get_string(string * dest,string * command_str,size_t * offset);

int parse_key(char * str, params_of_openfile * params); 
void initialization_params(params_of_openfile *);

size_t string_get_next_position(struct string * str,size_t offset);
void string_get(FILE * file_input,string * _str);

int string_get_number(string * command,size_t * offset);

char * get_next_word(string * str, size_t * offset);
/**************************************************/
long cartesian_size(struct cartesian_tree * _tree);
#endif
