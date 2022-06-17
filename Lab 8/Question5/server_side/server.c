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
#include <math.h>

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

int getWords(char *base, char target[3][8])
{
    int n = 0, i, j = 0;

    for (int i = 0; i < 3; i++)
    {
        target[i][0] = '0';
        target[i][1] = '\0';
    }

    for (i = 0; 1; i++)
    {
        if (base[i] != ' ')
        {
            target[n][j++] = base[i];
        }
        else
        {
            target[n][j++] = '\0'; // insert NULL
            n++;
            j = 0;
        }
        if (base[i] == '\0')
            break;
    }
    return n;
}

float math(char *arg)
{
    char arr[3][8];
    getWords(arg, arr);

    int a = atoi(arr[1]);
    int b = atoi(arr[2]);
    double z;
    // printf("%d, %d\n", a, b);

    // for (int i = 0; i < 3; i++)
    //     printf("%s\n", arr[i]);

    if (strcmp(arr[0], "ADD") == 0)
        z = a + b;
    if (strcmp(arr[0], "SUB") == 0)
        z = a - b;
    if (strcmp(arr[0], "MUL") == 0)
        z = a * b;
    if (strcmp(arr[0], "DIV") == 0)
        z = (float)a / b;

    if (strcmp(arr[0], "SIN") == 0)
    {
        double radian = a * (3.14 / 180);
        z = sin(radian);
    }
    if (strcmp(arr[0], "COS") == 0)
    {
        double radian = a * (3.14 / 180);
        z = cos(radian);
    }
    if (strcmp(arr[0], "TAN") == 0)
    {
        double radian = a * (3.14 / 180);
        z = tan(radian);
    }

    

    return z;
}

void Child(void *arg)
{
    char buffer[100];
    char str[100];
    int client = *(int *)arg;

    // STEP 1 SEND WELCOME MESSEGE

    send(client, "WELCOME TO THE SERVER\n", 22, 0);

    // STEP 2 REVC MATH COMMAND

    bzero(buffer, 100);
    recv(client, buffer, sizeof(buffer), 0);

    float res = math(buffer);   // FIND ANS

    // STEP 3 SEND ANS

    bzero(buffer, 100);
    bzero(str, 100);
    strncat(buffer, "Res ", 4);
    sprintf(str, "%.3f", res);
    strncat(buffer, str, strlen(str));

    send(client, buffer, strlen(buffer), 0);

    //STEP 4 RECV BYE

    bzero(buffer, 100);
    recv(client, buffer, 100, 0);
    printf("%s\n", buffer);

    //STEP 5 SEND GOOD BYE

    send(client, "GOOD BYE", 8, 0);




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