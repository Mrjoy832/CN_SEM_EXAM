// Modify the client so that it will continuously take echo service from the echo server. The client wiil do this untill it send ane receive string "EXIT" from and close connection.


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

#define MAXBUF 1024

int isSame(char out[], char buf[])
{
	if (strlen(buf) != strlen(out) + 1)
		return 0;

	for (int i = 0; i < strlen(out); i++)
		if (out[i] != buf[i])
			return 0;

	return 1;
}

int main(int Count, char *Strings[])
{
	int sock;
	struct sockaddr_in dest;
	char buf[MAXBUF];

	char out[] = "exit";

	if (Count != 3)
	{
		printf("USAGE: %s <server port number> <server IP address>\n", Strings[0]);
		exit(0);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);

	dest.sin_family = AF_INET;
	dest.sin_port = htons(atoi(Strings[2]));
	inet_aton(Strings[1], &dest.sin_addr);

	connect(sock, (struct sockaddr *)&dest, sizeof(dest));

	do
	{
		memset(buf, 0, MAXBUF);
		fgets(buf, MAXBUF, stdin);
		send(sock, buf, strlen(buf), 0);

		memset(buf, 0, MAXBUF);
		recv(sock, buf, MAXBUF, 0);
		printf("%s\n", buf);
	} while (!isSame(out, buf));

	close(sock);
}