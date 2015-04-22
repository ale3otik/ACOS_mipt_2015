
#ifndef _FILE_OPERATONS_
#define _FILE_OPERATONS_

#include "my_types.h"

#define F_BOTH 1
#define F_TO_READ 2
#define F_TO_WRITE 3
FILE * open_file( char * file_name,params_of_openfile * params,short state);
int read_file(FILE* fp, data_container ** data,params_of_openfile *params);
int write_to_file(char * cur_file_name,params_of_openfile * params,data_container * data);
#endif
