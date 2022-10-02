#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    pid_t child_pid[2];

    child_pid[0] = fork();
    if (child_pid[0] == -1)
    {
        printf("Не удалось выполнить fork().\n");
        return -1;
    }
    else if (child_pid[0] == 0)
    {
        execl("./lister", "lister", NULL);
    }

    child_pid[1] = fork();
    if (child_pid[1] == -1)
    {
        printf("Не удалось выполнить fork().\n");
        return -1;
    }
    else if (child_pid[1] == 0)
    {
        execl("./pysort.py", "pysort.py", NULL);
    }

    for (size_t i = 0; i < 2; i++)
    {
        int status;
        waitpid(child_pid[i], &status, 0);

        printf("Потомок с pid=%d ", child_pid[i]);
        if (WIFEXITED(status))
            printf("завершился с кодом %d.\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("остановлен сигналом с кодом %d\n", WTERMSIG(status));
        else if (WIFSTOPPED(status))
            printf("остановлен с кодом %d\n", WSTOPSIG(status));
    }

    return 0;
}
