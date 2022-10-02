#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        printf("Не удалось создать программный канал.\n");
        return -1;
    }

    const char* messages[] = {
        "123123.\n",
        "Ж123\n",
        "AAAAAAAAAAA\n",
        "xxx\n"
    };
    size_t n = sizeof(messages) / sizeof(messages[0]);
    for (size_t i = 0; i < n; i++) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            printf("Не удалось выполнить fork().\n");
            return -1;
        } else if (child_pid == 0) {
            close(pipefd[0]); // закрытие канала для чтения
            write(pipefd[1], messages[i], strlen(messages[i]));
            return 0;
        }
    }

    for (size_t i = 0; i < n; i++) {
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

    close(pipefd[1]); // закрытие канала на запись

    char buffer[256];
    read(pipefd[0], buffer, 256);
    printf("Предок получил сообщения от потомков:\n%s", buffer);

    return 0;
}
