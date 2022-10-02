#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void do_child_work(int fd)
{
    // write message into pipe channel

    FILE *file = fdopen(fd, "w");
    if (fprintf(file, "message from child!\n") < 0)
        printf("child can't send message!\n");
    else
        printf("child sent message!\n");
    fclose(file);
}

void do_parent_work(int fd)
{
    // read message from pipe channel

    FILE *file = fdopen(fd, "r");
    char buffer[256];
    if (fgets(buffer, 256, file) == NULL)
        printf("parent can't receive message!\n");
    else
    {
        printf("parent recieved message!\n");
        *strchr(buffer, '\n') = '\0';
        printf("message received: \"%s\"\n", buffer);
    }

    fclose(file);
}

int main(void)
{
    int fd[2];
    pid_t child_pid;

    if (pipe(fd) == -1)
        printf("can't pipe.\n");
    else if ((child_pid = fork()) == -1)
        printf("can't fork.\n");
    else if (child_pid == 0)
    {
        printf("child!\n");
        close(fd[0]);
        do_child_work(fd[1]);
        close(fd[1]);
    }
    else
    {
        printf("parent!\n");
        close(fd[1]);
        do_parent_work(fd[0]);
        close(fd[0]);
    }

    return 0;
}
