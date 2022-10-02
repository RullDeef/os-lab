#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILDS_NUM 5

int main(void)
{
    printf("Я - предок. pid: %d, gid: %d\n", getpid(), getpgrp());

    for (size_t i = 0; i < CHILDS_NUM; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == -1)
        {
            printf("Не удалось выполнить fork().\n");
            exit(EXIT_FAILURE);
        }
        else if (child_pid == 0)
        {
            printf("Я - потомок. pid: %d, ppid: %d, gid: %d\n", getpid(), getppid(), getpgrp());
            sleep(1);
            exit(EXIT_SUCCESS);
        }
        else
        {
            printf("Создан потомок с pid: %d\n", child_pid);
            wait(NULL);
            printf("Потомок с pid %d завершился.\n", child_pid);
        }
    }

    return EXIT_SUCCESS;
}
