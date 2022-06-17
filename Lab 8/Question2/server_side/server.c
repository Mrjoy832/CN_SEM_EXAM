#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>

void PANIC(char *msg);
#define PANIC(msg)   \
    {                \
        perror(msg); \
        exit(-1);    \
    }

void sig_handler(int sig)
{
    if (sig == SIGCHLD)
    {
        int retval;
        wait(&retval);
    }
}

int fileExist(char *filename)
{
    FILE *file;
    if (file = fopen(filename, "r"))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void Child(void *arg)
{
    char buffer[100];
    int client = *(int *)arg;
    
    	//SEND THE WELLCOME MESSEGE
    
    send(client, "WELCOME TO DOWNLOAD SERVER\n", 27, 0);

	//RECV THE FILE NAME

    bzero(buffer, 100);
    recv(client, buffer, 100, 0);
    
    	//CHECK THE FILE EXISTS OR NOT
    
    if (fileExist(buffer))
    {
    	//IF EXIST SEND OK AND SEND THE DATA
    	
        send(client, "OK", 2, 0);
        FILE *ptr = NULL;
        ptr = fopen(buffer, "r");
        char line[100];
        fgets(line, sizeof(line), ptr); // send data
        send(client, line, sizeof(line), 0);
        fclose(ptr);
    }
    else
    {
        send(client, "ERR", 3, 0);
    }


    close(client);
    exit(0);
}

int main()
{
    int sd;
    struct sigaction act;
    struct sockaddr_in addr;

    bzero(&act, sizeof(act));
    act.sa_handler = sig_handler;
    act.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &act, 0);

    if ((sd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        PANIC("socket");
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        PANIC("Bind");
    }
    if (listen(sd, 20) != 0)
    {
        PANIC("Listen");
    }

    while (1)
    {
        int client, addr_size = sizeof(addr);

        client = accept(sd, (struct sockaddr *)&addr, &addr_size);
        if (client < 0)
        {
            perror("Accept");
        }
        else
        {
            printf("Connected : %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            if (fork())
            {
                close(client);
            }
            else
            {
                close(sd);
                Child(&client);
                exit(0);
            }
        }
    }

    return 0;
}
