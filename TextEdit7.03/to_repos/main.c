#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "my_types.h"



int main(int argc,char ** argv)
{
    FILE * fp = NULL;  /* FILE structure*/
    char file_name[MAX_NAME_LENGTH] = "";
    data_container * data = NULL; /*main data container*/
    int next_action;
/**
* get start settings of editor
*/
    
    if(argc < 2)
    {
        file_name[0] = 0; /* No file create*/
    }
    else
    {
        strncpy(file_name,argv[1],MAX_NAME_LENGTH);
    }
    fp = open_file(file_name,F_BOTH); /* open or create file $name_file*/
/**********************END SETTINGS*****************************/
    read_file(fp,&data); /* get data*/
/*********************END READ********************************/
    next_action = general_function(file_name,data); /*run general program*/
    switch (next_action)
    {
        case EXIT:
            if(fp != NULL) fclose(fp);
            return 0;
        default:
            fprintf(stdout,"\nNEXT_ACTION IS NOT DEFINED\n");
            return -1;
    }

    return 0;
}







