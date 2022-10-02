#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MY_PORT 5555

int main(void)
{
    setbuf(stdout, NULL);

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_addr = (struct in_addr) { .s_addr = htonl(INADDR_LOOPBACK) },
        .sin_port = htons(MY_PORT)
    };

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("connect");
        exit(-1);
    }

    char buf[256];

    // while (1)
    // {
        printf("input message: ");
        fgets(buf, sizeof(buf), stdin);
        if (strlen(buf) > 0)
            buf[strlen(buf) - 1] = '\0';

        if (send(sock, buf, sizeof(buf), 0) <= 0)
            printf("failed sendto\n");
        else
        {
            printf("sent message!\n");

            int n = recv(sock, buf, sizeof(buf), 0);
            if (n == 0)
                printf("failed to recv response from server!\n");
            else
                printf("response: %s\n", buf);
        }
    // }

    close(sock);
    return 0;
}
