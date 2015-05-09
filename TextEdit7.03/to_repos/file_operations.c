#include "my_types.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FILE * open_file( char * file_name,short state)
{
    FILE* fp = NULL;
    if(!strncmp(file_name,"",MAX_NAME_LENGTH)) return fp;
    if(state == F_TO_READ || state == F_BOTH) 
    {
        fp = fopen(file_name,"r+"); /* try open to read +*/
    }
    if(fp == NULL)
    {
        if(state == F_TO_WRITE || state == F_BOTH)
        {
            fp = fopen(file_name,"w+"); /* if can't open to read try to write/create */
        }
        if(fp == NULL)
        {
            fprintf(stdout,"\nERROR: can't open file: %s\n",file_name);
        }
    }
    return fp;
}

int read_file(FILE* fp, data_container ** data)
{
    if(fp!= NULL)
    {
        int k = 0;
        char buf[MAX_STRING_LENGTH];
        string new_string;
        
        tree_delete(*data);
        *data = NULL;
        new_string.data = NULL; /* magic */  
        string_construct(&new_string); /* make or clear*/
        while(!feof(fp))
        {
            /*printf(".");*/
            do /* read one string (that can be long)*/
            {
                buf[0] = '\0';
                fgets(buf,MAX_STRING_LENGTH-1,fp);
                
                string_big_push(&new_string,buf,strlen(buf));
                if(feof(fp)) break;
            }while(buf[strlen(buf)-1] != '\n');
            /*getchar();*/
                cart_tree_insert(data,&new_string,k); /* add new string to data memory*/
               /* fprintf(stdout,"\n%ld\n",(*data)->size);*/
                ++k;
                string_construct(&new_string); /* clear and make new*/        
        }
        /*print_tree(stderr,*data);*/
        string_delete(&new_string);
        fclose(fp);
        
        return SUCCES;
    }
    return ERROR;
}

int write_to_file(char * cur_file_name,data_container * data)
{
     FILE * f_to_write = NULL;
    f_to_write = open_file(cur_file_name,F_TO_WRITE);
    if(f_to_write == NULL) return ERROR;

    print_tree(f_to_write,data);

    fclose(f_to_write);

    return SUCCES;
}


