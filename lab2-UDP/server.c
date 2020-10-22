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


void convert_and_print(char *b)
{
    int n = atoi(b);
    char res2[RES_LEN] = { 0 };
    convert_int(n, 2, res2);
        
    char res14[RES_LEN] = { 0 };
    convert_int(n, 14, res14);
                
    printf("decimal: %d\n", n);
    printf("binary: %s\n", res2);
    printf("octal: %o\n", n);
    printf("hexadecimal: %x\n", n);
    printf("14th: %s\n\n", res14); 
}


// a - число в 10й СС
// p - основание СС [2, 30]
// s - строка для результата
int convert_int(int a, int p, char *s)
{
    char letters[30] = {"0123456789ABCDEFGHIJKLMNOPQRST"};
    
    int num = (int)a;
    int rest = num % p;
    num /= p;
    if (num == 0)
    {
        s[0] = letters[rest]; 
        return 1;
    }
    int k = convert_int(num, p, s);
    s[k++] = letters[rest];
    return k;
}


int main(void)
{
    struct sockaddr_in server_addr, client_addr;
    int sock, slen = sizeof(client_addr);
    char buf[MSG_LEN];

    printf("Server started\n");

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        perror_and_exit("socket");

    memset((char *)&server_addr, 0, sizeof(server_addr));
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SOCK_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(sock, &server_addr, sizeof(server_addr)) == -1)
        perror_and_exit("bind");

    while (1)
    {
        if (recvfrom(sock, buf, MSG_LEN, 0, &client_addr, &slen) == -1)
            perror_and_exit("recvfrom()");
        
        printf("Received packet from %s:%d\n\n",
            inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        
        convert_and_print(buf);
	}

	close(sock);
	return 0;
}
