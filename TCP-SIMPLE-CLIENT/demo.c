//PORT=21

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
#define FTP_PORT 21

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
    /*if ( inet_aton(Strings[1], &dest.sin_addr) == 0 )
    {
    perror(Strings[1]);
    exit(errno);
    }*/
    dest.sin_addr = *((struct in_addr *)he->h_addr);
    dest.sin_port = htons(FTP_PORT);
    // printf("---------------------- here\n");





    /*--- Connect to server---*/
    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) != 0)
    {

        perror("Connect");
        exit(errno);
    }
    // printf("Connected to %s at port %d\n",inet_ntoa(dest.sin_addr), ntohs(dest.sin_port));
    /*--- Receive and print the welcome message from server ---*/
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);




    /* Take user name from user and send it to server */
    printf("Name :");
    bzero(str, 100);
    fgets(str, 100, stdin);
    bzero(buffer, MAXBUF);
    strncat(buffer, "USER ", 5);
    strncat(buffer, str, strlen(str));
    // printf("%s\n",buffer);
    send(sockfd, buffer, strlen(buffer), 0);




    /* receive and print the response from server */
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);



    /* Take password from user and send it to server */
    printf("Password :");
    bzero(str, 100);
    fgets(str, 100, stdin);
    bzero(buffer, MAXBUF);
    strncat(buffer, "PASS ", 5);
    strncat(buffer, str, strlen(str));
    // printf("%s\n",buffer);
    send(sockfd, buffer, strlen(buffer), 0);




    /* receive and print the response from server */
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);
    bzero(buffer, MAXBUF);
    strncat(buffer, "SYST\r\n", 6);
    strncat(buffer, str, strlen(str));
    // printf("%s\n",buffer);
    send(sockfd, buffer, strlen(buffer), 0);

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);
    do
    {
        printf("client>");
        bzero(str, 100);
        fgets(str, 100, stdin);
        if (!strncmp(str, "quit", 4))
        {
            bzero(buffer, MAXBUF);
            strncat(buffer, "QUIT\r\n", 6);
            // printf("%s\n",buffer);
            send(sockfd, buffer, strlen(buffer), 0);
            // printf("receiving ........\n");
            bzero(buffer, MAXBUF);
            recv(sockfd, buffer, MAXBUF, 0);
            printf("%s\n", buffer);
            break;
        }
        else
        {
            printf("Invalid command try again!!\n");
            continue;
        }
    } while (1);
    /*---Clean up---*/
    close(sockfd);
    return 0;
}