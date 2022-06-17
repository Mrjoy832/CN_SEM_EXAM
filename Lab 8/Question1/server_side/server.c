#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <stdlib.h>

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

int finduser(char *user)
{
    FILE *ptr = NULL;
    ptr = fopen("myfile.txt", "r");
    char line[50];
    char line2[50];
    strncat(user, "\n", 1);
    while (fgets(line, sizeof(line), ptr))
    {
        fgets(line2, sizeof(line2), ptr);
        if (strcmp(user, line) == 0)
        {
            return 1;
        }
    }
    fclose(ptr);
    return 0;
}

int auth(char *user, char *pass)
{
    FILE *ptr = NULL;
    ptr = fopen("myfile.txt", "r");
    char line[50];
    char line2[50];
    strncat(pass, "\n", 1);
    while (fgets(line, sizeof(line), ptr))
    {
        fgets(line2, sizeof(line2), ptr);
        if (strcmp(user, line) == 0 && strcmp(pass, line2) == 0 )
        {
            return 1;
        }
    }
    fclose(ptr);
    return 0;
}

void Child(void *arg)
{
    char line[100];
    char user[15];
    char pass[15];
    int byte_read;
    int client = *(int *)arg;
    send(client, "WELCOME\nEnter the username\n", 27, 0);

    while (1)
    {
        bzero(user, 15);
        recv(client, user, 15, 0);
        if (finduser(user) == 1)
        {
            printf("server side : user matched\n");
            send(client, "1Enter password\n", 16, 0);
            break;
        }
        else
        {
            printf("server side : user not found\n");
            send(client, "0Invalid user enter a valid username\n", 37, 0);
        }
    }

    while (1)
    {
        bzero(pass, 15);
        recv(client, pass, 15, 0);
        if (auth(user, pass) == 1)
        {
            send(client, "1Login\n", 7, 0);
            break;
        }
        else
        {
            send(client, "0Invalid password enter again\n", 30, 0);
        }
    }
    printf("%s", user);
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