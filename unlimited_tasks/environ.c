#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
/*
 * Он должен распечатать переметры программы, 
 * которые ей подавали на вход, и ещё 
 * массив кажется называется char **envp,
 * а __environ - это какой-то хак.
 */
int main(int argc, char* argv[], char* env[])
{
    int i;
    
    for (i = 0; i<argc; ++i)
    {
        puts(*argv);
    }
    
    puts("\nenviron:\n");
    
    i = 0;
    while(env[i])
    {
        puts(env[i]);
        ++i;
    }
    return 0;
}
