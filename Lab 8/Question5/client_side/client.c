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

    // STEP 1 RECV WELCOME MESSEGE

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s", buffer);

    // STEP 2 SEND MATH COMMAND

    bzero(str, 100);
    printf("Enter math command\n");
    gets(str);
    send(sockfd, str, sizeof(str), 0);

    // STEP 3 RECV ANS

    bzero(buffer, 100);
    recv(sockfd, buffer, 100, 0);
    printf("%s\n", buffer);

    //STEP 4 SEND BYE

    send(sockfd, "BYE", 3, 0);

    //STEP 5 RECV GOOD BYE

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    close(sockfd);
    return 0;
}