#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    printf("Предок:  pid=%d gid=%d\n", getpid(), getpgrp());

    for (size_t i = 0; i < 5; i++) {
        pid_t child_pid = fork();
        if (child_pid == -1) {
            printf("Не удалось выполнить fork().\n");
            return -1;
        } else if (child_pid == 0) {
            printf("Потомок: pid=%d ppid=%d gid=%d\n", getpid(), getppid(), getpgrp());
            sleep(1);
            printf("pid=%d ppid=%d gid=%d\n", getpid(), getppid(), getpgrp());
            return 0;
        }
    }

    return 0;
}
