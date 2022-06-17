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
	
	//RECV WELCOME MESSEGE
	
    bzero(buffer, MAXBUF);
    recv(sockfd, buffer, MAXBUF, 0);
    printf("%s\n", buffer);

	//SEND FILENAME

    bzero(buffer, MAXBUF);
    bzero(str, 100);
    printf("Enter the file name you want to download\n");
    gets(str);
    strncat(buffer, str, strlen(str));
    send(sockfd, buffer, sizeof(buffer), 0); // send the file name

	//RECV STATUS OF THE FILE EXISTANCE

    bzero(buffer, MAXBUF); // receive the status that the file exist or not
    recv(sockfd, buffer, MAXBUF, 0);


	//FILE CREATION AND RECV FILE DATA

    if (strcmp(buffer, "OK") == 0)
    {
        FILE *ptr = NULL;
        ptr = fopen(str, "w"); // file creation

        bzero(buffer, MAXBUF);
        recv(sockfd, buffer, sizeof(buffer), 0); // recv file data 
        fputs(buffer, ptr);
    }
    else if (strcmp(buffer, "ERR") == 0)
    {
        printf("The file isn't available");
    }

    close(sockfd);
    return 0;
}
