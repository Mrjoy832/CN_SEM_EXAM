/*
Write a C client program that will connect with a server running at port 143 and
implement the following dialog.

*/

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#define MAXBUF 1024
#define PORT_NUM 143

int main(int Count, char *Strings[])
{
    int sockfd, bytes_read;
    struct sockaddr_in dest; // socket address
    struct hostent *he;      //
    char buffer[MAXBUF];
    char str[100];

    /*---Make sure we have the right number of parameters---*/
    if (Count != 2)
    {
        fprintf(stderr, "usage: %s <server's hostname>\n", Strings[0]);
        exit(0);
    }

    /*--- Create socket for streaming ---*/
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket");
        exit(errno);
    }
    printf("Socket created\n");
    if ((he = gethostbyname(Strings[1])) == NULL)
    { /* get the host info */
        perror("gethostbyname");
        exit(1);
    }

    /*--- Initialize server address/port struct ---*/
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_addr = *((struct in_addr *)he->h_addr);
    dest.sin_port = htons(PORT_NUM);

    /*--- Connect to server---*/
    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) != 0)
    {
        perror("Connect");
        exit(errno);
    }
    /*--- Receive and print the welcome message from server ---*/
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    //_______________________________________________________________a1

    /* Take user name from user and send it to server */
    bzero(buffer, MAXBUF);
    strncat(buffer, "a1 LOGIN ", 9);

    printf("User : ");
    bzero(str, 100);
    scanf("%s", str);
    strncat(buffer, str, strlen(str));
    strncat(buffer, " ", 1);

    printf("Password : ");
    bzero(str, 100);
    scanf("%s", str);
    strncat(buffer, str, strlen(str));
    strncat(buffer, "\r\n", 2);

    send(sockfd, buffer, strlen(buffer), 0);

    /* receive and print the response from server */
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    //_______________________________________________________________a2

    bzero(buffer, MAXBUF);
    strncat(buffer, "a2 LIST \"\" \"*\"\r\n", 16);
    send(sockfd, buffer, strlen(buffer), 0);

    /* receive and print the response from server */
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    //_______________________________________________________________a3

    bzero(buffer, MAXBUF);
    strncat(buffer, "a3 EXAMINE INBOX\r\n", 18);
    send(sockfd, buffer, strlen(buffer), 0);

    /* receive and print the response from server */
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    //_______________________________________________________________a4

    printf("Enter mail no. To retrieve: ");

    bzero(str, 100);
    scanf("%s", str);

    bzero(buffer, MAXBUF);
    strncat(buffer, "a4 FETCH ", 20);
    strncat(buffer, str, strlen(str));
    strncat(buffer, " BODY[]\r\n", 20);
    send(sockfd, buffer, strlen(buffer), 0);

    /* receive and print the response from server */
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    //_______________________________________________________________a5

    bzero(buffer, MAXBUF);
    strncat(buffer, "a5 LOGOUT\r\n", 11);
    send(sockfd, buffer, strlen(buffer), 0);

    /* receive and print the response from server */
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    //_________________________________________________________________

    close(sockfd);
    return 0;
}

