#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "procinfo.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("Использование: %s <путь_программы> <имя_вых_файла>\n", argv[0]);
        return -1;
    }

    int pid = fork();
    if (pid == -1)
    {
        printf("Ошибка fork.\n");
        return -2;
    }
    else if (pid == 0)
    {
        int status = execl(argv[1], argv[1], NULL);
        if (status == -1)
        {
            printf("Ошибка exec.\n");
            return -3;
        }
        return status;
    }

    sleep(2);

    printf("proc pid: %d\n", pid);
    return scan_proc_info(pid, argv[2]);
}
