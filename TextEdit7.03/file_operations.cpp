#include "my_types.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

FILE * open_file( char * file_name,params_of_openfile * params,short state)
{
    FILE* fp = NULL;
    if(!strncmp(file_name,"",MAX_NAME_LENGTH)) return fp;
    if(params->binary_file == FALSE)
    {
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
                fprintf(stderr,"\nERROR: can't open file: %s\n",file_name);
            }
        }
    }
    else    /*binary file*/
    {
        if(state == F_TO_READ || state == F_BOTH) 
        {
            fp = fopen(file_name,"rb+"); /* try open to read +*/
        }
        if(fp == NULL)
        {
            if(state == F_TO_WRITE || state == F_BOTH)
            {
                fp = fopen(file_name,"wb+"); /* if can't open to read try to write/create */
            }
            if(fp == NULL)
            {
                fprintf(stderr,"\nERROR: can't open file: %s\n",file_name);
            }
        }
    }
    return fp;
}

int read_file(FILE* fp, data_container ** data,params_of_openfile *params)
{
    if(fp!= NULL)
    {
        int k = 0;
        char buf[MAX_STRING_LENGTH];
        tree_delete(*data);
        string new_string;
        *data = NULL;
        new_string.data = NULL; /* magic */  
        string_construct(&new_string); /* make or clear*/
        if(params->binary_file != TRUE)
        {
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
                   /* fprintf(stderr,"\n%ld\n",(*data)->size);*/
                    ++k;
                    string_construct(&new_string); /* clear and make new*/        
            }
        }
        else{}
        /*print_tree(stderr,*data);*/
        string_delete(&new_string);
        fclose(fp);
        
        return SUCCES;
    }
    return ERROR;
}

int write_to_file(char * cur_file_name,params_of_openfile * params,data_container * data)
{
     FILE * f_to_write = NULL;
    f_to_write = open_file(cur_file_name,params,F_TO_WRITE);
    if(f_to_write == NULL) return ERROR;

    print_tree(f_to_write,data);

    fclose(f_to_write);

    return SUCCES;
}


