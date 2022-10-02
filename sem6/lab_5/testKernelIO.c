#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

void* task(void *data)
{
    int fd = open("alphabet.txt", O_RDONLY);

    for (char c; read(fd, &c, 1) == 1;)
        write(1, &c, 1);

    close(fd);
    return NULL;
}

int main(void)
{
    pthread_t tid1, tid2;
    if (pthread_create(&tid1, NULL, task, NULL) != 0 ||
        pthread_create(&tid2, NULL, task, NULL) != 0)
    {
        perror("pthread_create");
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    write(1, "\n", 1);
    return 0;
}
