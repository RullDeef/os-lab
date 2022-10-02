#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
//#define __USE_MISC
#include <dirent.h>
#include "procinfo.h"

#define BUFF_SIZE 4096

#define with_opened_file(f, fname) \
    char __proc_fname[FILENAME_MAX]; \
    snprintf(__proc_fname, FILENAME_MAX, "/proc/%d/" fname, pid); \
    FILE *f = fopen(__proc_fname, "r"); \
    if (f == NULL) { \
        printf("Не удалось открыть файл: %s\n", __proc_fname); \
        return -1; \
    } else

#define read_symlink(link, fname) \
    char __proc_fname[FILENAME_MAX]; \
    snprintf(__proc_fname, FILENAME_MAX, "/proc/%d/" fname, pid); \
    char link[BUFF_SIZE]; \
    if (readlink(__proc_fname, link, BUFF_SIZE) == -1) { \
        printf("Не удалось открыть файл: %s\n", __proc_fname); \
        return -1; \
    } else

#define read_file(pf, outf, separator) \
    char __buf[BUFF_SIZE]; \
    int len; \
    while ((len = fread(__buf, 1, BUFF_SIZE, pf)) > 0) \
    { \
        for (size_t i = 0; i < len; i++) \
            if (__buf[i] == '\0') \
                __buf[i] = separator; \
        __buf[len] = '\0'; \
        fprintf(outf, "%s", __buf); \
    }

static int fscan_proc_cmdline(int pid, FILE* out_file);
static int fscan_proc_environ(int pid, FILE* out_file);
static int fscan_proc_stat(int pid, FILE* out_file);
static int fscan_proc_maps(int pid, FILE* out_file);
static int fscan_proc_comm(int pid, FILE* out_file);
static int fscan_proc_io(int pid, FILE* out_file);
static int fscan_proc_fd(int pid, FILE* out_file);
static int fscan_proc_root(int pid, FILE* out_file);
static int fscan_proc_wchan(int pid, FILE* out_file);
static int fscan_proc_task(int pid, FILE* out_file);

int scan_proc_info(int pid, const char* out_fname)
{
    FILE *out_file = fopen(out_fname, "w");
    if (out_file == NULL)
    {
        printf("Ошибка открытия выходного файла.\n");
        return -1;
    }
    else
    {
        int status = fscan_proc_cmdline(pid, out_file);
        if (status == 0)
            status = fscan_proc_environ(pid, out_file);
        if (status == 0)
            status = fscan_proc_stat(pid, out_file);
        if (status == 0)
            status = fscan_proc_maps(pid, out_file);
        if (status == 0)
            status = fscan_proc_comm(pid, out_file);
        if (status == 0)
            status = fscan_proc_io(pid, out_file);
        if (status == 0)
            status = fscan_proc_fd(pid, out_file);
        if (status == 0)
            status = fscan_proc_root(pid, out_file);
        if (status == 0)
            status = fscan_proc_wchan(pid, out_file);
        if (status == 0)
            status = fscan_proc_task(pid, out_file);

        fclose(out_file);
        return status;
    }
}

static int fscan_proc_cmdline(int pid, FILE* out_file)
{
    with_opened_file(proc_file, "cmdline")
    {
        fprintf(out_file, "=== [cmdline] ===\n\n");
        read_file(proc_file, out_file, ' ');
        fprintf(out_file, "\n\n");

        fclose(proc_file);
    }

    return 0;
}


static int fscan_proc_environ(int pid, FILE* out_file)
{
    with_opened_file(proc_file, "environ")
    {
        fprintf(out_file, "=== [environ] ===\n\n");
        read_file(proc_file, out_file, '\n');
        fprintf(out_file, "\n");

        fclose(proc_file);
    }

    return 0;
}

static int fscan_proc_stat(int pid, FILE* out_file)
{
    with_opened_file(proc_file, "stat")
    {
        fprintf(out_file, "=== [stat] ===\n\n");
        read_file(proc_file, out_file, ' ');
        fprintf(out_file, "\n\n");

        fclose(proc_file);
    }

    return 0;
}

static int fscan_proc_maps(int pid, FILE* out_file)
{
    with_opened_file(proc_file, "maps")
    {
        fprintf(out_file, "=== [maps] ===\n\n");
        read_file(proc_file, out_file, ' ');
        fprintf(out_file, "\n\n");

        fclose(proc_file);
    }

    return 0;
}

static int fscan_proc_comm(int pid, FILE* out_file)
{
    with_opened_file(proc_file, "comm")
    {
        fprintf(out_file, "=== [comm] ===\n\n");
        read_file(proc_file, out_file, ' ');
        fprintf(out_file, "\n\n");

        fclose(proc_file);
    }

    return 0;
}

static int fscan_proc_io(int pid, FILE* out_file)
{
    with_opened_file(proc_file, "io")
    {
        fprintf(out_file, "=== [io] ===\n\n");
        read_file(proc_file, out_file, ' ');
        fprintf(out_file, "\n\n");

        fclose(proc_file);
    }

    return 0;
}

static int fscan_proc_fd(int pid, FILE* out_file)
{
    char dir_name[FILENAME_MAX];
    snprintf(dir_name, FILENAME_MAX, "/proc/%d/fd/", pid);

    DIR* dp = opendir(dir_name);
    if (dp == NULL)
    {
        printf("Не удалось открыть директорию /proc/%d/fd\n", pid);
        return -1;
    }
    else
    {
        struct dirent* dent;

        fprintf(out_file, "=== [fd] ===\n\n");

        int status = 0;
        while (status == 0 && (dent = readdir(dp)) != NULL)
        {
            if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
                continue;

            char buff[BUFF_SIZE];
            memset(buff, 0, BUFF_SIZE);
            snprintf(dir_name, FILENAME_MAX, "/proc/%d/fd/%s", pid, dent->d_name);
            readlink(dir_name, buff, BUFF_SIZE);
            fprintf(out_file, "%s -> %s\n", dent->d_name, buff);
        }

        fprintf(out_file, "\n\n");
        closedir(dp);

        return status;
    }
}

static int fscan_proc_root(int pid, FILE* out_file)
{
    read_symlink(link, "root")
    {
        fprintf(out_file, "=== [root] ===\n\n");
        fprintf(out_file, "/proc/%d/root -> %s\n\n", pid, link);
    }

    return 0;
}

static int fscan_proc_wchan(int pid, FILE* out_file)
{
    with_opened_file(proc_file, "wchan")
    {
        fprintf(out_file, "=== [wchan] ===\n\n");
        read_file(proc_file, out_file, ' ');
        fprintf(out_file, "\n\n");

        fclose(proc_file);
    }

    return 0;
}

static int fscan_proc_task(int pid, FILE* out_file)
{
    char dir_name[FILENAME_MAX];
    snprintf(dir_name, FILENAME_MAX, "/proc/%d/task", pid);

    DIR* dp = opendir(dir_name);
    if (dp == NULL)
    {
        printf("Не удалось открыть директорию /proc/%d/task\n", pid);
        return -1;
    }
    else
    {
        struct dirent* dent;

        fprintf(out_file, "=== [task] ===\n\n");

        int status = 0;
        while (status == 0 && (dent = readdir(dp)) != NULL)
        {
            if (strcmp(dent->d_name, ".") == 0 || strcmp(dent->d_name, "..") == 0)
                continue;

            // char buff[BUFF_SIZE];
            // memset(buff, 0, BUFF_SIZE);
            // snprintf(dir_name, FILENAME_MAX, "/proc/%d/task/%s", pid, dent->d_name);
            // readlink(dir_name, buff, BUFF_SIZE);
            fprintf(out_file, "%s\n", dent->d_name);
        }

        fprintf(out_file, "\n\n");
        closedir(dp);

        return status;
    }
}
