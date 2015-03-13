#include <stdio.h>
#include <unistd.h>

/*
 * Он должен распечатать переметры программы, 
 * которые ей подавали на вход, и ещё 
 * массив кажется называется char **envp,
 * а __environ - это какой-то хак.
 */
int main()
{
    int i;
    for(i=0;__environ [i] != NULL;++i)
    {
        printf("%s\n" , __environ [i]);
        ++i;
    }	            
    return 0;
}
