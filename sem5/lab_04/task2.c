#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(void) {
    const size_t child_num = 5;

    printf("Предок: pid=%d gid=%d\n", getpid(), getpgrp());

    for (size_t i = 0; i < child_num; i++) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            printf("Не удалось выполнить fork().\n");
            return -1;
        } else if (child_pid == 0) {
            printf("Потомок №%ld: pid=%d ppid=%d gid=%d\n", i, getpid(), getppid(), getpgrp());
            return 100 + i;
        }
    }

    for (size_t i = 0; i < child_num; i++) {
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
