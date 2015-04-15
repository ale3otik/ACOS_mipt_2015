
#ifndef _FILE_OPERATIONS_H_
#define _FILE_OPERATIONS_H_

#include <unistd.h>
#include <stdlib.h>		
#include "my_types.h"

#define F_TO_READ 1
#define F_TO_WRITE 2
#define F_BOTH 3 
FILE * open_file(char*,params_of_openfile*,short);
int read_file(FILE * ,data_container ** , params_of_openfile *);
int write_to_file(char * cur_file_name,params_of_openfile * params,data_container * data);

#endif
