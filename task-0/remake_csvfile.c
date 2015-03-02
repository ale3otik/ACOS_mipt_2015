#include <stdlib.h>
#include <stdio.h>
int main(int argc,char ** argv)
{
    FILE *fp;
    char c;
    int n;
    if(argc != 2)
    {
        printf("error name of file");
        exit(1);
    }
    if(!(fp = fopen(argv[1],"r+")))
    {
        printf("file error");
        exit(1);
    }
    n = 0;
    while(!feof(fp))
    {
        if(fgetc(fp) == '.')
        {
            fseek(fp,n,SEEK_SET);
            fputc(',',fp);
        }
        ++n;
    }
    fclose(fp);
    return 0;
}
