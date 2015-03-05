#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
void swap(int * Arr, const int i,const int j)
{
    int buf;
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
int * create_shuffle(const int length)
{
    int * Arr;
    int i,j;
    assert(Arr = (int*)malloc(sizeof(int)*length));
    for(i = 0;i<length;i++)
    {
        Arr[i] = i;
    }
    for(i = length - 1; i>=0;--i)
    {
        j = rand() % (i+1);
        swap(Arr,i,j);
    }
    return Arr;
}
float InitLine(int * Array, const int length)
{
    long timeInit = clock();
    int i;
    long summ = 0;
    for(i = 0; i< length; ++i)
    {
        summ += Array[i];
    }
   return (clock() - timeInit)/ (float)(CLOCKS_PER_SEC);
}
float InitLineM(const int M, int * Array, const int length)
{
    int start = clock();
    int i,j=0;
    long summ=0;
    for(j = 0;j<M;++j)
    {
        for(i = j;i<length;i+= M)
        {
            summ += Array[i];
        }
    }
    return (clock() - start)/ (float)(CLOCKS_PER_SEC);
}
float InitLineRand(int * Arr,int * Shuffle, const int length)
{
    int i;
    long start = clock();
    long summ = 0;
    for(i = 0;i<length;++i)
    {
        summ += Arr[Shuffle[i]];
    }
    free(Shuffle);
    return (clock() - start)/ (float)(CLOCKS_PER_SEC);
}
int main(int argc,char ** argv)
{
    int length; /*LENGTH OF ARAY*/
    int * Array;
    int M; /*param to gallop*/
    float timeIn[3];
    FILE * fp;
    timeIn[0] = -1;
    timeIn[1] = -1;
    timeIn[2] = -1;
    if(argc < 4 || argc > 5)
    {
        printf("error_number of arguments");
        exit(1);
    }
    fp = initFile(argv[1]);
    length = atoi(argv[2]);
    M = atoi(argv[3]);
    srand(time(NULL)); /* update random()*/
    assert(Array = (int*)malloc(sizeof(int)*length));
    Array = create_shuffle(length);
    if(argc != 5 || (argc == 5 && strcmp(argv[4],"gallop") != 0))
    {
        timeIn[0] = InitLine(Array,length);
        timeIn[2] = InitLineRand(Array,create_shuffle(length),length);
    }
    timeIn[1] = InitLineM(M,Array,length);
    if(argc != 5 || (argc == 5 && strcmp(argv[4],"gallop") != 0))
    {
        fprintf(fp,"%d;%d;%.3f;%.3f;%.3f;\n",length,M,timeIn[0],timeIn[1],timeIn[2]);
    }
    else
    {
        fprintf(fp,"%d;%d;%.3f;\n",length,M,timeIn[1]);
    }
    free(Array);
    fclose(fp);
    return 0;
}
