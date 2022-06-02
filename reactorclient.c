/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT "3492" // the port client will be connecting to

int sockfd, numbytes;
int flag;

void *sendFun(void *arg)
{
    char buffer[1024];
    bzero(buffer, 1024);
    while (flag != 0)
    {
        //printf("Send: ");
        scanf("%s", buffer);
        if (strncmp(buffer, "Exit", 4) == 0)
        {
            send(sockfd, "Exit", 4, 0);
            flag = 0;
            break;
        }
        if (send(sockfd, buffer, strlen(buffer) + 1, 0) == -1)
        {
            perror("send");
        }
        bzero(buffer, 1024);
    }
    return NULL;
}

void *recvFun(void *arg)
{
    char buffer[1024];
    bzero(buffer, 1024);
    int bytes = 0;
    while ((bytes = recv(sockfd, buffer, 1024, 0)) != -1)
    {
        if (!bytes)
        {
            flag = 0;
            break;
        }
        printf("Recieved: %s\n", buffer);
        if (!strcmp(buffer, "Exit"))
        {
            flag = 0;
            break;
        }
        bzero(buffer, 1024);
    }
    return NULL;
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2)
    {
        fprintf(stderr, "usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL)
    {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    pthread_t thread1, thread2;
    flag = 1;
    pthread_create(&thread1, NULL, recvFun, NULL);
    pthread_create(&thread2, NULL, sendFun, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_kill(thread2, 0);
    close(sockfd);
    printf("Bye\n");
    return 0;
}