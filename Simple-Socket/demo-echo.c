//demo simple echo client
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <errno.h>
#include <unistd.h>

#define MAXBUF	1024

int main(int Count, char *Strings[])
{	
	int sock;
	struct sockaddr_in dest;
	char buf[MAXBUF];
	
	if(Count != 3){
		printf("USAGE: %s <server port number> <server IP address>\n",Strings[0]);
		exit(0);
		
	}
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(Strings[2]));
	inet_aton(Strings[1], &dest.sin_addr);
	
	connect(sock, (struct sockaddr *)&dest, sizeof(dest));
	
	memset(buf, 0, MAXBUF);
	fgets(buf, MAXBUF, stdin);
	send(sock, buf, strlen(buf), 0);
	
	memset(buf, 0, MAXBUF);
	recv(sock, buf, MAXBUF, 0);
	printf("%s\n",buf);
	
	close(sock);
}