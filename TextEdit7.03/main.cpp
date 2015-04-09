#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "myTypes.h"



int main(int argc,char ** argv)
{
    FILE * fp = NULL;  /* FILE structure*/
    params_of_openfile params; /* to parse keys*/
    char file_name[MAX_NAME_LENGTH] = "";
    data_container * data = NULL; /*main data container*/
    int next_action;
    int i;
/**
* get start settings of editor
*/

    initialization_params(&params); /*set default to params*/
    
    if(argc < 2)
    {
        file_name[0] = 0; /* No file create*/
    }
    else
    {
        for(i = 1; i < argc && argv[i][0] == '-';++i)
        {
            parse_key(argv[i],&params);
        }
        if(i < argc)
        {
            strncpy(file_name,argv[i],MAX_NAME_LENGTH);
        }
        /*else
        {
            fprintf(stderr,"\nERROR: where is my file?\n");
            exit(1);
        }*/
    }
    
    
    fp = open_file(file_name,&params,F_BOTH); /* open or create file $name_file*/
/**********************END SETTINGS*****************************/
    read_file(fp,&data,&params); /* get data*/
/*********************END READ********************************/
    next_action = general_function(file_name,data,&params); /*run general program*/
    switch (next_action)
    {
        case EXIT:
            tree_delete(data);
            
            return 0;
        default:
            fprintf(stderr,"\nNEXT_ACTION IS NOT DEFINED\n");
            return -1;
    }

/************************************************************/

 /*MAKE SAVING*/
/************************************************************/
    return 0;
}







