#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

void do_parent_work(void)
{
    FILE *file = fopen("pp", "r");

    if (file == NULL)
    {
        printf("parent can't open pipe.\n");
        exit(-1);
    }

    char buffer[256];
    while (fgets(buffer, 256, file) != NULL)
    {
        if (strchr(buffer, '\n') != NULL)
            *strchr(buffer, '\n') = '\0';
        printf("massage received: \"%s\"\n", buffer);
    }

    fclose(file);
}

void do_child_work(void)
{
    FILE *file = fopen("pp", "w");

    if (file == NULL)
    {
        printf("child can't open pipe.\n");
        exit(-1);
    }

    fprintf(file, "I\n");
    fprintf(file, "AM\n");
    fprintf(file, "CHILD\n");

    fclose(file);
}

int main(void)
{
    pid_t child_pid = fork();

    if (child_pid == -1)
        printf("can't fork.\n");
    else if (child_pid == 0)
    {
        printf("child!\n");
        do_child_work();
    }
    else
    {
        printf("parent!\n");
        do_parent_work();
    }

    return 0;
}
