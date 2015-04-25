
#ifndef _FILE_OPERATONS_
#define _FILE_OPERATONS_

#include "my_types.h"
FILE * open_file( char * file_name,short state);
int read_file(FILE* fp, data_container ** data);
int write_to_file(char * cur_file_name,data_container * data);
#endif
