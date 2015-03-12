#include <stdio.h>
#include <unistd.h>
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
