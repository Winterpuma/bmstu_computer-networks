#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>

#include "../server.h"

#define BUFLEN 500


std::string generate_get(std::string filename)
{
    const std::string version = "HTTP/1.1";
    return "GET /" + filename + " " + version + "\r\n" + "User: console-pid-" + std::to_string(getpid());
}


void perror_and_exit(std::string msg)
{
    perror(msg);
    exit(-1);
}


int main(void)
{
    srand(time(NULL));

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        perror_and_exit("Error in sock");

    struct hostent* host = gethostbyname(SOCK_ADDR);
    if (!host)
        perror_and_exit("Error in gethostbyname");

    // Заполенине sockaddr
    struct sockaddr_in server_addr;
    server_addr.sin_family = PF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    server_addr.sin_addr = *((struct in_addr*) host->h_addr_list[0]);

    if (connect(sock, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0)
        perror_and_exit("Error in connect");

    std::string request = generate_get("index.html");
    if (send(sock, request.c_str(), request.length(), 0) < 0)
        perror_and_exit("Error in send");

    printf("Client has send a get request, waiting for response...\n");

    char buf[BUFLEN];
    if (recv(sock, buf, BUFLEN, 0) < 0)
        perror_and_exit("Error in recv");

    printf("Client has recieved an answer:\n\n%s", buf);

    close(sock);
    return 0;
}
