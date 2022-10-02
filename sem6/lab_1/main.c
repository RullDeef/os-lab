#define _POSIX_C_SOURCE 200809L

#include <fcntl.h>          // fcntl
#include <pthread.h>        // pthread
#include <sys/stat.h>       // umask
#include <sys/resource.h>   // getrlimit
#include <signal.h>         // signal, sigset_t
#include <unistd.h>         // open, close

#include <errno.h>          // errno
#include <stdarg.h>         // va_args
#include <stdio.h>          // printf, perror
#include <stdlib.h>         // exit
#include <string.h>         // strlen
#include <time.h>           // time, localtime

#define LOG_FILE_PATH "/home/rulldeef/Projects/os-lab/sem6/lab_1/file.log"
#define LOCKFILE_PATH "/dev/daemon.pid"
#define O_LOGGER (O_CREAT | O_WRONLY | O_APPEND)
#define S_IRWUGO (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH)


int thread_running = 1;
int log_file_fd;

void daemonize(void);
int already_running(void);

int lockfile(int fd);

void* thread_fn(void* arg);

void sig_handler(int sig);

void log_msg(const char* format, ...);

int main(void) {
    int sig;
    sigset_t sigset;
    pthread_t tid;

    daemonize();

    log_file_fd = open(LOG_FILE_PATH, O_LOGGER, S_IRWUGO);
    if (log_file_fd == -1) {
        return -1;
    }

    if (already_running()) {
        log_msg("Демон уже запущен.\n");
        return -1;
    }

    if (pthread_create(&tid, NULL, thread_fn, NULL) == -1) {
        log_msg("ошибка при создании потока\n");
        return -1;
    }

    log_msg("демон готов к работе...\n");

    signal(SIGTERM, sig_handler);
    signal(SIGHUP, sig_handler);
    
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGHUP);
    sigaddset(&sigset, SIGTERM);

    while (1) {
        if (sigwait(&sigset, &sig) == 0) {
            if (sig == SIGTERM) {
                log_msg("Зарегистирован сигнал SIGTERM. Завершение работы демона...\n");
                break;
            } else if (sig == SIGHUP) {
                log_msg("Зарегистирован сигнал SIGHUP.\n");
            }
        } else {
            log_msg("Bad sigwait: %s", strerror(errno));
        }
    }

    thread_running = 0;
    exit(0);
}

void daemonize(void) {
    int fd;
    struct rlimit rl;
    pid_t pid;
    struct sigaction sa;

    umask(0);

    if ((pid = fork()) == -1) {
        perror("fork");
        exit(1);
    } else if (pid != 0)
        exit(0);

    setsid();

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        perror("sigaction");
        exit(-1);
    }

    if (chdir("/") == -1) {
        perror("chdir");
        exit(1);
    }

    if (getrlimit(RLIMIT_NOFILE, &rl) == -1) {
        perror("getrlimit");
        exit(1);
    }

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (fd = 0; fd < rl.rlim_max; fd++)
        close(fd);
}

int already_running(void) {
    int fd;
    char buf[32];

    fd = open(LOCKFILE_PATH, O_CREAT | O_WRONLY);

    if (fd < 0) {
        perror("open");
        return -1;
    }

    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return -1;
        }
        perror("lockfile");
        return -1;
    }

    snprintf(buf, sizeof(buf), "%d", getpid()); 
    write(fd, buf, strlen(buf) + 1);
    return 0;
}

int lockfile(int fd) {
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;

    return fcntl(fd, F_SETLK, &fl);
}

void* thread_fn(void* arg) {
    time_t t;

    while (thread_running) {
        t = time(NULL);
        log_msg("daemon PID=%d time: %s", getpid(), asctime(localtime(&t)));
        sleep(1);
    }

    return NULL;
}

void sig_handler(int sig) {
    if (sig == SIGTERM) {
        thread_running = 0;
    }
}

void log_msg(const char* format, ...) {
    char buffer[1024];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    write(log_file_fd, buffer, strlen(buffer));
    fsync(log_file_fd);
}
