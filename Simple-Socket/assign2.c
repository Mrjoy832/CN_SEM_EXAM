// 2. Take the server IP address and the range of port number from command line and report which port within that range on that server are open with  protocol name.


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

#define MAXBUF 1024

int main(int Count, char *Strings[])
{

	if(Count != 4){
		printf("USAGE: %s <server IP address> <starting port number> <ending port number>\n",Strings[0]);
		exit(0);
	}


    int sockfd;
    struct sockaddr_in dest;
    char buffer[MAXBUF];


    int startport = atoi(Strings[2]);
    int endport = atoi(Strings[3]);


    /*---Initialize server address struct---*/
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    if (inet_aton(Strings[1], &dest.sin_addr) == 0)
    {
        perror(Strings[1]);
        exit(errno);
    }



    for (int i = startport; i <= endport; i++)
    {

        /*---Open socket for streaming---*/
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("Socket");
            exit(errno);
        }

        dest.sin_port = htons(i);  //Initialize server port number

        /*---Connect to server---*/
        if (connect(sockfd, (struct sockaddr *)&dest, sizeof(dest)) == 0)
        {
            printf("%d\tOpen\n", i);
        }
    }

    close(sockfd);
    return 0;
}