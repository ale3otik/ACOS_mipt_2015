#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void add_to_arr(char ** array,int k,int size, char c)
{
    if(k>= size)
    {
        *array = realloc(*array,size*2);
        if(*array == NULL) exit(4);
        
        size *= 2;
    }
    (*array)[k]  = c;
}

int main(int argc,char * argv[])
{
    if(argc<2) return 0;
    FILE * fp;
    char name[1000];
    
    int cur =0;
    char c;
    char * array;
    char * delete;
    int length = argc -2;
    int check;
    int size = 20;
    
    int i;
    int k;
    strncpy(name,argv[1],1000);
    fp = fopen(argv[1],"rb");
    if(fp == NULL) exit(1);
    delete = (char*)malloc(sizeof(char) * length);
    if(delete == NULL) exit(2);
    
    array = (char*)malloc(sizeof(char)*20);
    if(array == NULL) exit(3);
    
    for(i = 2;i<argc;++i)
    {
        delete[i-2] = atoi(argv[i]);
    }
    
    while(1)
    {
        c = fgetc(fp);
        if(feof(fp)) break;
        
        check = 0;
        for(i = 0;i<length;++i)
        {
            if(c == delete[i])
            {
                check = 1;
                break;
            }
        }
        if(check != 1)
        {
            add_to_arr(&array,k,size,c);
            ++k;
        }
        
    }
    fclose(fp);
    fp = fopen(name,"wb");
    for(i = 0;i<k;++i)
    {
        fputc(array[i],fp);
    }
    
    free(array);
    free(delete);
    
    /*РАБОТАЕТ!!!*/
    return 0;
}
