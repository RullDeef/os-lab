#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/stat.h>

pthread_mutex_t mutex;

void describe(FILE* fp, const char* prefix)
{
    int pos = ftell(fp);
    struct stat s;
    if (stat("output.txt", &s) != 0)
    {
        perror("stat");
        exit(-1);
    }
    printf("%s: inode = %ld, size = %ld, pos = %d\n",
        prefix, s.st_ino, s.st_size, pos);
}

void write_syms(FILE* fp, char begin, char end)
{
    while (begin != end)
        fprintf(fp, "%c", begin++);
}

void* task(void* data)
{
    FILE* fp = fopen("output.txt", "at");
    describe(fp, "fopen");

    char buffer[10];
    setvbuf(fp, buffer, _IOFBF, sizeof(buffer));

    if (data == NULL)
        write_syms(fp, 'a', 'z' + 1);
    else
        write_syms(fp, '0', '9' + 1);

    describe(fp, "fclose");
    fclose(fp);
    return NULL;
}

int main(void)
{
    pthread_mutex_init(&mutex, NULL);

    pthread_t tid1, tid2;
    if (pthread_create(&tid1, NULL, task, NULL) != 0 ||
        pthread_create(&tid2, NULL, task, (void*)1) != 0)
    {
        perror("pthread_create");
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    return 0;
}
