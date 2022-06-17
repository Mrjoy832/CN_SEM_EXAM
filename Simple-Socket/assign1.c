// 1. Take the server IP address and port number from command line and perform the same.

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>

#define PORT_TIME 13 /* "time" (not available on RedHat) */
// #define PORT_FTP        21              /* FTP connection port */
// #define SERVER_ADDR     "192.168.43.12"     /* localhost */
#define MAXBUF 1024

int main(int Count, char *Strings[])
{
    int sockfd;
    struct sockaddr_in dest;
    char buffer[MAXBUF];

    if (Count != 3)
    {
        printf("USAGE: %s <server IP address> <server port number>\n", Strings[0]);
        exit(0);
    }

    /*---Open socket for streaming---*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket");
        exit(errno);
    }

    /*---Initialize server address/port struct---*/
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(atoi(Strings[2]));
    if (inet_aton(Strings[1], &dest.sin_addr) == 0)
    {
        perror(Strings[1]);
        exit(errno);
    }

    /*---Connect to server---*/
    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) != 0)
    {
        perror("Connect ");
        exit(errno);
    }

    /*---Get "Hello?"---*/
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    /*---Clean up---*/
    close(sockfd);
    return 0;
}