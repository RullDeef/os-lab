#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_SOCKET_PATH "./server.sock"
#define CLIENT_SOCKET_PATH "./client.sock"

void send_request(int sock)
{
    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = SERVER_SOCKET_PATH
    };

    char buf[256];
    snprintf(buf, sizeof(buf), "request from pid=%d", getpid());

    if (sendto(sock, buf, sizeof(buf), 0, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        printf("error when sending request message\n");
        return;
    }

    printf("sended request from pid=%d\n", getpid());

    if (recvfrom(sock, buf, sizeof(buf), 0, NULL, NULL) == -1)
        printf("error when receiving response\n");
    else
        printf("received response: \"%s\"\n", buf);
}

int main(void)
{
    int sock = socket(AF_UNIX, SOCK_DGRAM, 0);

    struct sockaddr_un addr = {
        .sun_family = AF_UNIX,
        .sun_path = CLIENT_SOCKET_PATH
    };

    if (sock == -1)
    {
        printf("failed to create socket\n");
        return -1;
    }
    else if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        printf("bind error\n");
    }
    else
    {
        printf("sending request...\n");
        send_request(sock);
    }

    close(sock);
    remove(CLIENT_SOCKET_PATH);
    return 0;
}
