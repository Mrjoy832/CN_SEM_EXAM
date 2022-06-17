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

void Child(void *arg)
{
    char buffer[100];
    int client = *(int *)arg;

    //STEP 1 send welcome messege
    send(client, "WELCOME\n", 8, 0);

    //STEP 2 recv file name
    bzero(buffer, 100);
    recv(client, buffer, 100, 0);
    FILE *ptr = NULL;
    ptr = fopen(buffer, "w");

    //STEP 3 send ok status
    send(client, "OK", 3, 0);

    //STEP 4 recv file data
    bzero(buffer, 100);
    recv(client, buffer, 100, 0);
    fputs(buffer, ptr);

    //STEP 5 send bye
    send(client, "BYE\n", 4, 0);

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
