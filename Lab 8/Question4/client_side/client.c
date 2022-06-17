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

    // STEP 2 SEND USERNAME

    bzero(str, 100);
    printf("Enter username\n");
    gets(str);
    send(sockfd, str, sizeof(str), 0);
    // send(sockfd, "pritam", 6, 0);

    // STEP 3 READ STATUS OF USERNAME

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    if (strcmp(buffer, "0") == 0)
    {
        printf("USER NOT FOUND\n");
        exit(0);
    }
    // STEP 4 SEND PASSWORD

    bzero(str, 100);
    printf("Enter password\n");
    gets(str);
    send(sockfd, str, sizeof(str), 0);
    // send(sockfd, "pritam123", 9, 0);

    // STEP 5 READ STATUS OF PASSWORD

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    if (strcmp(buffer, "0") == 0)
    {
        printf("PASSWORD DOSN'T MATCH\n");
        exit(0);
    }

    // STEP 6 SEND DATA

    bzero(buffer, MAXBUF);
    bzero(str, 100);
    printf("Enter information\n");
    gets(str);
    strncat(buffer, str, sizeof(str));
    send(sockfd, buffer, sizeof(buffer), 0);

    // STEP 7 RECV BYE

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    close(sockfd);
    return 0;
}