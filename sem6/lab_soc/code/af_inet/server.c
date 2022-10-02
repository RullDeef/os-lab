#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_CLIENT_NUM 16

#define MY_PORT 5555

int sock;
int clientfd_buf[MAX_CLIENT_NUM];

void invert_case(char* buf)
{
    while (*buf)
    {
        if (islower(*buf))
            *buf = toupper(*buf);
        else if (isupper(*buf))
            *buf = tolower(*buf);
        buf++;
    }
}

int init_fd_set(fd_set* fds)
{
    int maxfd = sock;

    FD_ZERO(fds);
    FD_SET(sock, fds);
    for (int i = 0; i < MAX_CLIENT_NUM; i++)
    {
        if (clientfd_buf[i] == 0)
            continue;

        FD_SET(clientfd_buf[i], fds);
        if (clientfd_buf[i] > maxfd)
            maxfd = clientfd_buf[i];
    }
    
    return maxfd;
}

void add_new_connection(int clientfd)
{
    for (int i = 0; i < MAX_CLIENT_NUM; i++)
    {
        if (clientfd_buf[i] == 0)
        {
            clientfd_buf[i] = clientfd;
            break;
        }
    }
}

void remove_connection(int clientfd)
{
    for (int i = 0; i < MAX_CLIENT_NUM; i++)
    {
        if (clientfd_buf[i] == clientfd)
        {
            clientfd_buf[i] = 0;
            break;
        }
    }
}

int main(void)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);

    if (sock == -1)
    {
        perror("error when creating socket");
        return -1;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = (struct in_addr) { .s_addr = htonl(INADDR_LOOPBACK) },
        .sin_port = htons(MY_PORT)
    };

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
        perror("bind");
    else
    {
        listen(sock, 1);
        printf("listening for client messages...\n");

        fd_set fds;

        while (1)
        {
            int maxfd = init_fd_set(&fds);
            struct timeval timeout = {
                .tv_sec = 100,
                .tv_usec = 0
            };

            int select_res = select(maxfd + 1, &fds, NULL, NULL, &timeout);
            if (select_res == -1)
            {
                perror("select");
                break;
            }
            else if (select_res == 0)
            {
                printf("still waiting...\n");
                continue;
            }

            // handle new connection
            if (FD_ISSET(sock, &fds))
            {
                printf("new connection\n");
                int clientfd = accept(sock, NULL, NULL);
                if (clientfd == -1)
                    perror("accept");
                else
                    add_new_connection(clientfd);
            }

            for (int i = 0; i < MAX_CLIENT_NUM; i++)
            {
                int clientfd = clientfd_buf[i];

                if (clientfd == 0)
                    continue;
                
                if (FD_ISSET(clientfd, &fds))
                {
                    char buf[256];

                    if (recv(clientfd, buf, sizeof(buf), 0) == 0)
                    {
                        printf("client disconnected\n");
                        remove_connection(clientfd);
                    }
                    else
                    {
                        printf("recieved message: %s\n", buf);
                        invert_case(buf);
                        if (send(clientfd, buf, sizeof(buf), 0) == -1)
                            printf("failed to send response\n");
                        else
                            printf("sent message: %s\n", buf);
                    }
                }
            }
        }
    }

    close(sock);
    return 0;
}
