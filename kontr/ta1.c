#include <stdio.h>
#include <stdlib.h>

long int func(long int x,long int y,long int n, long int p)
{
    int L = sizeof(long int);
    long int mask = 1;
    int i;
    
    y = y>>(p-n);
    for(i = 0;i<L;++i)
    {
        
        if(i>L-p && i <= (L-p)+n)
        {
            ;
        }
        else
        {
            y = y|mask;
            mask=mask<<1;
        }
    }
    mask = 1;
    for(i = 0;i<L;++i)
    {
        if(i>L-p && i<= (L-p)+n)
        {
            x = x|mask;
        }
        mask = mask << 1;
    }
    x = x&y;
    return x;
}
int main()
{
    /*scanf("%d",x);*/
    printf("%d",func(255,0,5,5));
    
    /* ДОЛЖНА РАБОТАТЬ*/
    return 0;
}
