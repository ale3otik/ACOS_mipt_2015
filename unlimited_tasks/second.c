#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#define BUF_SIZE 100

int main(int argc,char ** argv)
{
    int id = -1;
    char buf[BUF_SIZE];
    int n_bytes;
    
    if(argc > 1)
    {
        id = creat(argv[1],0600);
        if(id<0)
        {
            write(3,"\ncan't open file",strlen("\ncan't open file") * sizeof(char));
            return -1;
        }
    }
    else
    {
        id = 1;
    }
    dup2(id,1);
    if(id != 1) close(id);
    
    while(1)
    {
        n_bytes = read(0,buf,BUF_SIZE*sizeof(char));
        if(n_bytes == 0) break;
        /*if(buf[0] == '\t') break;*/
        write(1,buf,n_bytes * sizeof(char));
    }
    close(1);
    return 0;
}
