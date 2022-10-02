#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>

int write_enabled = 0;

void child_sig_handler(int sig)
{
    write_enabled = 1;
}

int main(void)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        printf("Не удалось создать программный канал.\n");
        return -1;
    }

    const char* messages[] =
    {
        "XXX",
        "aaaaaaa",
    };

    for (size_t i = 0; i < 2; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == -1)
        {
            printf("Не удалось выполнить fork().\n");
            return -1;
        }
        else if (child_pid == 0)
        {
            signal(SIGINT, child_sig_handler);
            usleep(100000 * (i + 2));
            if (write_enabled)
            {
                close(pipefd[0]);
                write(pipefd[1], messages[i], strlen(messages[i]));
            }
            return 0;
        }
    }

    close(pipefd[1]);
    signal(SIGINT, SIG_IGN);

    char buffer[256] = {'\0'};
    memset(buffer, '\0', sizeof(buffer));
    if (read(pipefd[0], buffer, sizeof(buffer)))
        printf("сообщение: %s\n", buffer);
    else
        printf("больше сообщений нет\n");

    memset(buffer, '\0', sizeof(buffer));
    if (read(pipefd[0], buffer, sizeof(buffer)))
        printf("сообщение: %s\n", buffer);
    else
        printf("больше сообщений нет\n");

    memset(buffer, '\0', sizeof(buffer));
    if (read(pipefd[0], buffer, sizeof(buffer)))
        printf("сообщение: %s\n", buffer);
    else
        printf("больше сообщений нет\n");

    for (size_t i = 0; i < 2; i++)
    {
        int status;
        pid_t child_pid = wait(&status);
        printf("Потомок с pid=%d ", child_pid);

        if (WIFEXITED(status))
            printf("завершился с кодом %d.\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))
            printf("остановлен сигналом с кодом %d\n", WTERMSIG(status));
        else if (WIFSTOPPED(status))
            printf("остановлен с кодом %d\n", WSTOPSIG(status));
    }

    return 0;
}
