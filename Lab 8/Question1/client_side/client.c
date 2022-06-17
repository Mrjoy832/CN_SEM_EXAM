#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>

#define MAXBUF 1024

int main(int count, char *Strings[])
{
    int sockfd, byte_read;
    struct sockaddr_in dest;
    char buffer[MAXBUF];
    char str[100];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket");
        exit(errno);
    }

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    if (inet_aton(Strings[1], &dest.sin_addr) == 0)
    {
        perror(Strings[1]);
        exit(errno);
    }
    dest.sin_port = htons(atoi(Strings[2]));

    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) != 0)
    {
        perror("Connect");
        exit(errno);
    }

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    while (1) // username
    {
        bzero(buffer, MAXBUF);
        bzero(str, 100);
        printf("enter user : ");
        scanf("%s", str);
        strncat(buffer, str, strlen(str));

        send(sockfd, buffer, strlen(buffer), 0);

        bzero(buffer, MAXBUF);
        recv(sockfd, buffer, MAXBUF, 0);
        printf("%s\n", buffer);
        if (buffer[0] == '1')
        {
            break;
        }
    }

    while (1) // password
    {
        bzero(buffer, MAXBUF);
        bzero(str, 100);
        printf("enter password : ");
        scanf("%s", str);
        strncat(buffer, str, strlen(str));

        send(sockfd, buffer, strlen(buffer), 0);

        bzero(buffer, MAXBUF);
        recv(sockfd, buffer, MAXBUF, 0);
        printf("%s\n", buffer);
        if (buffer[0] == '1')
        {
            break;
        }
    }

    close(sockfd);
    return 0;
}