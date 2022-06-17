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

int finduser(char *user)
{
    FILE *ptr = NULL;
    ptr = fopen("user.txt", "r");
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
    ptr = fopen("user.txt", "r");
    char line[50];
    char line2[50];
    strncat(pass, "\n", 1);
    while (fgets(line, sizeof(line), ptr))
    {
        fgets(line2, sizeof(line2), ptr);
        if (strcmp(user, line) == 0 && strcmp(pass, line2) == 0)
        {
            return 1;
        }
    }
    fclose(ptr);
    return 0;
}

void Child(void *arg)
{
    char buffer[100];
    char user[100];
    char pass[100];
    int client = *(int *)arg;

    // STEP 1 SEND WELCOME MESSEGE

    send(client, "WELCOME TO THE SERVER\n", 22, 0);

    // STEP 2 REVC USERNAME AND CHECK IN USER.TXT

    bzero(buffer, 100);
    recv(client, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    // STEP 3 SEND 1 OR 0 CORROSPONDING TO VALID OR NOT

    bzero(user, 100);
    strncat(user, buffer, sizeof(buffer));
    // send(client, "1", 1, 0);

    if (finduser(user))
    {
        send(client, "1", 1, 0);
    }
    else
    {
        send(client, "0", 1, 0);
    }

    // STEP 4 RECV PASSWWORD AND CHECK IN USER.TXT

    bzero(buffer, 100);
    recv(client, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    // STEP 5 SEND 2 OR 0 CORROSPONDING TO VADIL OR NOT

    bzero(pass, 100);
    strncat(pass, buffer, sizeof(buffer));
        // send(client, "1", 1, 0);

    if (auth(user, pass))
    {
        send(client, "1", 1, 0);
    }
    else
    {

        send(client, "0", 1, 0);
    }

    // STEP 6 CREATE INFO.TXT RECV FILE DATA AND WRITE IT TO THE INFO.TXT

    FILE *ptr = NULL;
    ptr = fopen("info.txt", "w");

    bzero(buffer, 100);
    recv(client, buffer, sizeof(buffer), 0);
    printf("%s\n", buffer);

    fputs(buffer, ptr);
    fclose(ptr);

    // STEP 7 BYE

    send(client, "BYE", 3, 0);

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