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

    // STEP 1 RECV welcome messege
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s", buffer);

    // STEP 2 SEND file name
    bzero(str, 100);
    printf("Enter file name \n");
    scanf("%s", str);
    bzero(buffer, MAXBUF);
    strncat(buffer, str, strlen(str));
    send(sockfd, buffer, strlen(buffer), 0);

    // STEP 3 recv ok status
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    if (strcmp(buffer, "OK") != 0)
    {
        exit(0);
    }

    // STEP 4 send file data
    FILE *ptr = NULL;
    ptr = fopen(str, "r");
    fgets(str, sizeof(str), ptr);
    send(sockfd, str, sizeof(str), 0);

    // STEP 5 recv bye
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s", buffer);

    close(sockfd);
    return 0;
}
