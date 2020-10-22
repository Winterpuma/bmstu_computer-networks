
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include "settings.h"

void perror_and_exit(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    struct sockaddr_in server_addr;
    int sock, slen = sizeof(server_addr);
    char buf[MSG_LEN];

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        perror_and_exit("socket");

    memset((char *)&server_addr, 0, sizeof(server_addr)); // заполняем нулями
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);              // преобразует в сетевой порядок расположения байтов

    if (inet_aton(SRV_IP, &server_addr.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    printf("Input message: ");
    
    fgets(buf, MSG_LEN, stdin);
    if (sendto(sock, buf, MSG_LEN, 0, &server_addr, slen) == -1)
        perror_and_exit("sendto()");

    close(sock);
    return 0;
}
