// ЛР3 по UNIX.
// Производство-потребление

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h>

#define N       26  // размер буфера pp_buf
#define N_CONS  5   // число процессов-потребителей
#define N_PROD  4   // число процессов-производителей

// номера семафоров в массиве
#define S_BUFFER_FULL   0
#define S_BUFFER_EMPTY  1
#define S_BIN_SEM       2

#define PERMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

struct sembuf sop_cons_lock[] = {
    { S_BUFFER_FULL, -1, 0 },
    { S_BIN_SEM, -1, 0 },
};

struct sembuf sop_cons_unlock[] = {
    { S_BIN_SEM, 1, 0 },
    { S_BUFFER_EMPTY, 1, 0 },
};

struct sembuf sop_prod_lock[] = {
    { S_BUFFER_EMPTY, -1, 0 },
    { S_BIN_SEM, -1, 0 },
};

struct sembuf sop_prod_unlock[] = {
    { S_BIN_SEM, 1, 0 },
    { S_BUFFER_FULL, 1, 0 },
};

void consumer_work(int semid, int shmid)
{
    int cons_no = getpid() - getppid();
    char *pp_buf = shmat(shmid, NULL, 0);
    int flag;

    do
    {
        usleep((200 + rand() % 200) * 1000);

        if (semop(semid, sop_cons_lock, 2) == -1)
            perror("Потребитель, lock");

        flag = pp_buf[N + 1] < N;
        if (flag)
        {
            char sym = pp_buf[pp_buf[N + 1]];
            pp_buf[N + 1]++;
            flag = pp_buf[N + 1] < N;

            printf("\tПотребитель №%d: %c\n", cons_no, sym);
        }

        if (semop(semid, sop_cons_unlock, 2) == -1)
            perror("Потребитель, unlock");
    } while (flag);

    shmdt(pp_buf);
}

void producer_work(int semid, int shmid)
{
    int prod_no = getpid() - getppid();
    char *pp_buf = shmat(shmid, NULL, 0);
    int flag;

    do
    {
        usleep((200 + rand() % 200) * 1000);

        if (semop(semid, sop_prod_lock, 1) == -1)
            perror("Производитель, lock");

        // критическая секция
        flag = pp_buf[N] < N;
        if (flag)
        {
            char sym = 'A' + pp_buf[N];
            pp_buf[pp_buf[N]] = sym;
            pp_buf[N]++;

            printf("Производитель №%d: %c\n", prod_no, sym);
        }

        if (semop(semid, sop_prod_unlock, 2) == -1)
            perror("Производитель, unlock");
    } while (flag);
    
    shmdt(pp_buf);
}

void run_consumers(int semid, int shmid)
{
    for (int i = 0; i < N_CONS; i++)
    {
        pid_t child = fork();
        if (child == -1)
            perror("Не удалось породить потребителя");
        else if (child == 0)
        {
            consumer_work(semid, shmid);
            exit(0);
        }
    }
}

void run_producers(int semid, int shmid)
{
    for (int i = 0; i < N_PROD; i++)
    {
        pid_t child = fork();
        if (child == -1)
            perror("Не удалось породить производителя");
        else if (child == 0)
        {
            producer_work(semid, shmid);
            exit(0);
        }
    }
}

int main(void)
{
    int shmid;  // id дескриптора разделяемой области памяти
    int semid;  // id массива трёх семафоров: buffer_full, buffer_empty, bin_sem

    // создать разделяемую память для буфера
    shmid = shmget(IPC_PRIVATE, N + 2, IPC_CREAT | PERMS);
    if (shmid == -1)
    {
        perror("Не удалось создать разделяемую область памяти");
        return -1;
    }

    // создать массив семафоров
    semid = semget(IPC_PRIVATE, 3, IPC_CREAT | PERMS);
    if (semid == -1)
    {
        perror("Не удалось создать массив семафоров");
        return -1;
    }

    // инициализация памяти
    char *buf = shmat(shmid, NULL, 0);
    memset(buf, 0, N + 2);
    shmdt(buf);

    // инициализация массива семафоров
    if (semctl(semid, S_BUFFER_FULL, SETVAL, 0) == -1 ||
        semctl(semid, S_BUFFER_EMPTY, SETVAL, N) == -1 ||
        semctl(semid, S_BIN_SEM, SETVAL, 1) == -1)
            perror("Ошибка инициализации семафоров");
    else
    {
        run_consumers(semid, shmid);
        run_producers(semid, shmid);
        
        for (int i = 0, status; i < N_CONS + N_PROD; i++)
        {
            pid_t pid = wait(&status);
        }
    }

    // помечаем массив семафоров для удаления
    if (semctl(semid, 3, IPC_RMID) == -1)
        perror("Ошибка управления семафорами");

    // помечаем разделяемую память для удаления
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        perror("Ошибка управления разделяемой областью памяти");

    return 0;
}
