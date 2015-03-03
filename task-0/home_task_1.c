#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
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
float InitLine(int * Array, const int LENGTH,int * Values)
{
    int timeInit = clock();
    int i,j;
    for(i = 0; i< LENGTH; ++i)
    {
        Array[i] = Values[i];
    }
    free(Values);
   return (float)(clock() - timeInit)/ (CLOCKS_PER_SEC);
}
float InitLineM(const int M, int * Array, const int LENGTH,int * Values)
{
    int start = clock();
    int i,j;
    for(j = 0;j<M;++j)
    {
        for(i = j;i<LENGTH;i+= M)
        {
            Array[i] = Values[i];
        }
    }
    free(Values);
    return (float)(clock() - start)/ (CLOCKS_PER_SEC);
}
float InitLineRand(int * Arr,int * Shuffle, int LENGTH)
{
    int i;
    int start = clock();
    for(i = 0;i<LENGTH;++i)
    {
        Arr[Shuffle[i]] = Shuffle[i];
    }
    free(Shuffle);
    return (float)(clock() - start)/ (CLOCKS_PER_SEC);
}
int main(int argc,char ** argv)
{
    int LENGTH; /*LENGTH OF ARAY*/
    int * Array;
    int M; // param to gallop
    int startTime;
    float timeIn[3];
    if(argc != 4)
    {
        printf("error_number of arguments");
        exit(1);
    }
    FILE * fp = initFile(argv[1]);
    LENGTH = atoi(argv[2]);
    M = atoi(argv[3]);
    srand(time(NULL)); /* update random()*/
    startTime = clock();
    assert(Array = (int*)malloc(sizeof(int)*LENGTH));
    timeIn[0] = InitLine(Array,LENGTH,create_shuffle(LENGTH));
    timeIn[1] = InitLineM(M,Array,LENGTH,create_shuffle(LENGTH));
    timeIn[2] = InitLineRand(Array,create_shuffle(LENGTH),LENGTH);
    fprintf(fp,"%d;%d;%.3f;%.3f;%.3f;\n",LENGTH,M,timeIn[0],timeIn[1],timeIn[2]);
    free(Array);
    fclose(fp);
    return 0;
}
