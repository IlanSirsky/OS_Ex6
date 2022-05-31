#include "mainQueue.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#define PORT "3491" // the port users will be connecting to
#define BACKLOG 10 // how many pending connections queue will hold


void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;

    errno = saved_errno;
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

void *myThreadFun(void *vargp)
{
    int *hold = (int *)vargp;
    int new_fd = *hold;

    sock = new_fd;

    char data[1024];
    int Bytes;

    int counter = 0;

    while (counter < 5)
    {
        
        if((Bytes = recv(new_fd, data, 1024, 0)) == -1)
        {
            perror("recv");
            exit(1);
        } 

        data[Bytes] = '\0';

        printf("RECIEVED: %s \n", data);
        // pthread_mutex_lock(&mutex);
        
        enQ(q1, data);   
        
        // pthread_mutex_unlock(&mutex);
        
        counter+=1;
    }

    AO * ao1 = (AO *)malloc(sizeof(AO));
    AO * ao2 = (AO *)malloc(sizeof(AO));
    AO * ao3 = (AO *)malloc(sizeof(AO));
    pthread_t t1, t2, t3;

    ao1->q = q1;
    ao1->qFun = caesarCipher;
    ao1->endFun = nextQueue1;
    ao1->t_id = t1;

    ao2->q = q2;
    ao2->qFun = changeCase;
    ao2->endFun = nextQueue2;
    ao2->t_id = t2;

    ao3->q = q3;
    ao3->qFun = sendNode;
    ao3->endFun = nodeCounter;
    ao3->t_id = t3;

    pipline *pipe = (pipline *)(malloc(sizeof(pipline)));
    
    pipe->first = ao1;
    pipe->second = ao2;
    pipe->third = ao3;

    pthread_create(&t1, NULL, newAO2, pipe->first);
    sleep(3);

    pthread_create(&t2, NULL, newAO2, pipe->second);
    sleep(3);

    pthread_create(&t3, NULL, newAO2, pipe->third);
    sleep(3);

    close(new_fd);
    return NULL;
}

void *startServer(){
    int sockfd; // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }

     // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1)
        {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1)
        {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL)
    {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1)
    {
        perror("listen");
        exit(1);
    }

     sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1)
    {
        perror("sigaction");
        exit(1);
    }

    printf("server: waiting for connections...\n");


    sin_size = sizeof their_addr;
    sock = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (sock == -1)
    {
        perror("accept");
    }

    inet_ntop(their_addr.ss_family,
                get_in_addr((struct sockaddr *)&their_addr),
                s, sizeof(s));
    printf("server: got connection from %s\n", s);

    pthread_t thread_id;
    pthread_create(&thread_id, NULL, myThreadFun, &sock);

    pthread_join(thread_id, NULL);


    return NULL;

}

int main(){
    q1 = createQ();
    q2 = createQ();
    q3 = createQ();
    
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, startServer, NULL);

    pthread_join(thread_id, NULL);

    return 0;
}
