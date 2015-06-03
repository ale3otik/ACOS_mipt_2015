#ifndef _FUNCTIONS_H_
#define _FUNCTIONS_H_

#include "my_types.h"
long string_get_next_position(struct string * str,long offset); /* to ignore spaces*/
int string_get(FILE * file_input,string * _str);
void get_program_info(program * prog, string * command, long * offset);
void program_destroy(program * prog);
void program_construct(program * prog);

#endif
