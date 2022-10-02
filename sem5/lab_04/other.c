#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    int childpid;
    if ((childpid = fork()) == -1)
    {
        perror("cant fork\n");
        return -1;
    }
    else if (childpid == 0)
    {
        while (1)
            printf("%d ", getpid());
        return 0;
    }
    else
    {
        while (1)
            printf("\t\t%d ", getpid());
        return 0;
    }
    return 0;
}
