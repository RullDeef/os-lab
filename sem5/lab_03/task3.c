#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t child_ls_pid, child_ps_pid;

    if ((child_ls_pid = fork()) == -1)
    {
        printf("Не удалось выполнить fork() для ls.\n");
        return EXIT_FAILURE;
    }
    else if (child_ls_pid == 0)
    {
        execlp("ls", "-la", NULL);
        fflush(stdout);
        return EXIT_SUCCESS;
    }
    else if ((child_ps_pid = fork()) == -1)
    {
        printf("Не удалось выполнить fork() для ps.\n");
        return EXIT_FAILURE;
    }
    else if (child_ps_pid == 0)
    {
        execlp("ps", "-la", NULL);
        fflush(stdout);
        return EXIT_SUCCESS;
    }

    wait(NULL);
    sleep(1);
    return EXIT_SUCCESS;
}
