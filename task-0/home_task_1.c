#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
void swap(int ** Arr, const int i,const int j)
{
    int * buf;
    if(i != j)
    {
        buf = Arr[i];
        Arr[i] = Arr[j];
        Arr[j] = buf;
    }
}
FILE * initFile(char * file_name)
{
   FILE * fp = fopen(file_name,"a+");
   if(!fp)
   {
            printf("FILE ERROR");
            exit(1);
   }
   return fp;
}
int * getValues(int length)
{
    int * Values;
    int i;
    if(!(Values = (int*)malloc(length* sizeof(int))))
    {
        printf("error valuse malloc");
        exit(1);
    }
    for(i = 0;i<length;++i)
    {
        Values[i] = rand()% 100 + 100;
    }
    return Values;
}
int ** create_shuffle(int * Values,const int length,int param)
{
    int ** index_arr;
    int i,j,k=0;
    int check = 0;
    if(!(index_arr = (int**)malloc(sizeof(int*)*length)))
    {
        printf("error shuffle malloc");
        exit(1);
    }
    switch (param)
    {
        case 0: 
            check = 1; /*random activate*/
            param = 1;
        default:
            for(i = 0;i<param;++i)
                {
                    for(j = i;j<length;j+=param)
                    {
                        index_arr[k] = &Values[j];
                        ++k;
                    }
                }
            if(k != length)
            {
                printf("error index of index_arr %d\n",k);
                exit(1);
            }
    }
    if(check == 1)
    {
        for(i = length - 1; i>=0;--i)
        {
            j = rand() % (i+1);
            swap(index_arr,i,j);
        }
    }
    return index_arr;
}
float CountLine(int ** index_arr, const int length)
{
    long timeInit = clock();
    int i;
    long summ = 0;
    for(i = 0; i< length; ++i)
    {
        summ += **(index_arr + i);
    }
    timeInit = clock() - timeInit;
    free(index_arr);
    return timeInit/(float)(CLOCKS_PER_SEC);
}
int main(int argc,char ** argv)
{
    int length; /*LENGTH OF ARAY*/
    int * Values;
    int M; /*param to gallop*/
    float timeIn[3];
    FILE * fp;
    srand(time(NULL)); /* update random()*/
    timeIn[0] = -1;
    timeIn[1] = -1;
    timeIn[2] = -1;
    if(argc < 4 || argc > 5)
    {
        printf("error_number of arguments");
        exit(1);
    }
    fp = initFile(argv[1]); /*open out files*/
    length = atoi(argv[2]);
    M = atoi(argv[3]);
    Values = getValues(length); /* main data */
    if(argc != 5 || (argc == 5 && strcmp(argv[4],"gallop") != 0))
    {
        timeIn[0] = CountLine(create_shuffle(Values,length,1),length);
        timeIn[2] = CountLine(create_shuffle(Values,length,0),length);
    }
    timeIn[1] = CountLine(create_shuffle(Values,length,M),length);
    if(argc != 5 || (argc == 5 && strcmp(argv[4],"gallop") != 0))
    {
        fprintf(fp,"%d;%d;%.3f;%.3f;%.3f;\n",length,M,timeIn[0],timeIn[1],timeIn[2]);
    }
    else
    {
        fprintf(fp,"%d;%d;%.3f;\n",length,M,timeIn[1]);
    }
    free(Values);
    fclose(fp);
    return 0;
}
