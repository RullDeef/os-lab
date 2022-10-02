// ЛР3 по UNIX.
// Читатели-писатели

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>

#define N_RDRS  5   // число процессов-читателей
#define N_WRTS  3   // число процессов-писателей

#define PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

// индексы семафоров в массиве
#define SEM_ACTIVE_RDRS 0
#define SEM_ACTIVE_WRITER 1
#define SEM_QUEUE_READ 2
#define SEM_QUEUE_WRIT 3

struct sembuf sop_start_read[] = {
    { SEM_QUEUE_READ, 1, 0 },
    { SEM_ACTIVE_WRITER, 0, 0 },
    { SEM_QUEUE_WRIT, 0, 0 },
    { SEM_ACTIVE_RDRS,  1, 0 },
    { SEM_QUEUE_READ, -1, 0 },
};

struct sembuf sop_stop_read[] = {
    { SEM_ACTIVE_RDRS, -1, 0 },
};

struct sembuf sop_start_write[] = {
    { SEM_QUEUE_WRIT, 1, 0 },
    { SEM_ACTIVE_RDRS,  0, 0 },
    { SEM_ACTIVE_WRITER, 0, 0 },
    { SEM_ACTIVE_WRITER, 1, 0 },
    { SEM_QUEUE_WRIT, -1, 0 },
};

struct sembuf sop_stop_write[] = {
    { SEM_ACTIVE_WRITER, -1, 0 },
};

void start_read(int semid)
{
    if (semop(semid, sop_start_read, 5) == -1)
        perror("semop");
}

void stop_read(int semid)
{
    if (semop(semid, sop_stop_read, 1) == -1)
        perror("semop");
}

void start_write(int semid)
{
    if (semop(semid, sop_start_write, 5) == -1)
        perror("semop");
}

void stop_write(int semid)
{
    if (semop(semid, sop_stop_write, 1) == -1)
        perror("semop");
}

void reader_loop(int rd_no, int shmid, int semid)
{
    int *var = shmat(shmid, NULL, 0);

    for (int i = 0; i < 10; i++)
    {
        usleep((50 + rand() % 250) * 1000);

        start_read(semid);

        printf("\t\tЧитатель №%d прочитал: %d\n", rd_no, *var);

        stop_read(semid);
    }

    shmdt(var);
}

void writer_loop(int wr_no, int shmid, int semid)
{
    int *var = shmat(shmid, NULL, 0);

    for (int i = 0; i < 10; i++)
    {
        usleep((50 + rand() % 250) * 1000);

        start_write(semid);

        ++*var;
        printf("Писатель №%d написал: %d\n", wr_no, *var);

        stop_write(semid);
    }

    shmdt(var);
}

void init_workers(int n, int shmid, int semid, void (*worker)(int, int, int))
{
    for (int i = 1; i <= n; i++)
    {
        pid_t child = fork();
        if (child == -1)
            perror("fork");
        else if (child == 0)
        {
            worker(i, shmid, semid);
            exit(0);
        }
    }
}

void wait_workers(int n)
{
    int rc;
    for (int i = 0; i < n; i++)
    {
        pid_t child = wait(&rc);
    }
}

int main(void)
{
    int shmid;
    int semid;

    // создать разделяемую область памяти
    shmid = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | PERMS);
    if (shmid == -1)
    {
        perror("shmget");
        return -1;
    }

    // создать набор семафоров
    semid = semget(IPC_PRIVATE, 4, IPC_CREAT | PERMS);
    if (semid == -1)
    {
        perror("semget");
        return -1;
    }

    // проинициализировать семафоры
    if (semctl(semid, SEM_ACTIVE_RDRS, SETVAL, 0) == -1 ||
        semctl(semid, SEM_ACTIVE_WRITER, SETVAL, 0) == -1 ||
        semctl(semid, SEM_QUEUE_READ, SETVAL, 0) == -1 ||
        semctl(semid, SEM_QUEUE_WRIT, SETVAL, 0) == -1)
    {
        perror("semctl");
        return -1;
    }

    init_workers(N_WRTS, shmid, semid, writer_loop);
    init_workers(N_RDRS, shmid, semid, reader_loop);

    wait_workers(N_RDRS + N_WRTS);

    if (semctl(semid, 0, IPC_RMID) == -1)
        perror("semctl");

    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        perror("shmctl");

    return 0;
}
