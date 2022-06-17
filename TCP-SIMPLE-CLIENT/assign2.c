//Write a C client program that will connect with a server running at port 110 and
//implement the following dialog.
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
#define FTP_PORT 110

int findIndexInStr(char str[], char substr[])
{
    int index = -1;
    for (int i = 0; str[i] != '\0'; i++){
        index = -1;
        for (int j = 0; substr[j] != '\0'; j++){
            if (str[i + j] != substr[j]){
                index = -1;
                break;
            }
            index = i;
        }
        if (index != -1)
            break;
    }
    return index;
}

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
    dest.sin_port = htons(FTP_PORT);


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



    //**********************************************************

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

    //**********************************************************

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

    //**********************************************************

    bzero(buffer, MAXBUF);
    strncat(buffer, "LIST\r\n", 6);
    // printf("%s\n",buffer);
    send(sockfd, buffer, strlen(buffer), 0);

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    //***********************************************************

    printf("Enter mail no. To retrieve: ");
    bzero(str, 100);
    fgets(str, 100, stdin);
    bzero(buffer, MAXBUF);
    strncat(buffer, "RETR ", 8);
    strncat(buffer, str, strlen(str));
    // printf("%s\n",buffer);
    send(sockfd, buffer, strlen(buffer), 0);

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    int i = findIndexInStr(buffer, "Date");
    char *string = malloc(strlen(buffer));
   
    printf("%s\n", strncpy(string, buffer + i, strlen(buffer) - i - 3));
    free(string);


    //*************************************************************
   
    bzero(buffer, MAXBUF);
    strncat(buffer, "QUIT\r\n", 6);
    send(sockfd, buffer, strlen(buffer), 0);

    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

    close(sockfd);
    return 0;
}

