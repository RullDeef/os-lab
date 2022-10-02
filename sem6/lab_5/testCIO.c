#define _XOPEN_SOURCE 600
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <pthread.h>

void* task(void *data)
{
    int fd = *(int*) data;

    FILE *fp = fdopen(fd, "r");
    char buffer[5];
    setvbuf(fp, buffer, _IOFBF, sizeof(buffer));

    for (char tmp; fscanf(fp, "%c", &tmp) == 1;)
        printf("%c", tmp);

    printf("\n");
    return NULL;
}

int main(void)
{
    pthread_t tid1, tid2;
    int fd = open("alphabet.txt", O_RDONLY);

    if (pthread_create(&tid1, NULL, task, (void *)&fd) != 0 ||
        pthread_create(&tid2, NULL, task, (void *)&fd) != 0)
    {
        perror("pthread_create");
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    close(fd);
    return 0;
}
