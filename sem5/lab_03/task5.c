#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main(void)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        printf("Не удалось создать программный канал.\n");
        exit(EXIT_FAILURE);
    }

    pid_t child_pid = fork();
    if (child_pid == -1)
    {
        printf("Не удалось выполнить fork().\n");
        exit(EXIT_FAILURE);
    }
    else if (child_pid == 0)
    {
        close(pipefd[0]);

        const char *message = "hello, from child\0";
        write(pipefd[1], message, strlen(message) + 1);

        close(pipefd[1]);
    }
    else
    {
        close(pipefd[1]);

        char buffer[256];
        read(pipefd[0], buffer, 256);

        printf("сообщение от потомка: \"%s\"\n", buffer);

        close(pipefd[0]);
    }

    return EXIT_SUCCESS;
}
