#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

// Queue implementation using linked list taken from https://www.geeksforgeeks.org/queue-linked-list-implementation/
// and modified to fit the needs of this project.

static int sock = -1;
static int counter = 0;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;

static struct Queue * q1;
static struct Queue * q2;
static struct Queue * q3;

typedef struct QNode
{
    void *data;
    struct QNode *next;
} QNode;

QNode * newQNode(void *data);

typedef struct Queue
{
    QNode *front, *rear;
} Queue;

Queue * createQ();

void *deQ(Queue *q);
void enQ(Queue *q, void *x);
void destroyQ(Queue *q);

typedef struct active_object {
    Queue *q;
    void *(*qFun)(void *);
    void *(*endFun)(void *);
    pthread_t t_id;
} AO;

void newAO(Queue *q, void *(*qFun)(void *), void *(*endFun)(void *));
void * newAO2(void * args);
void destroyAO(AO * ao);

typedef struct pipline
{
    AO *first;
    AO *second;
    AO *third;
} pipline;

void *caesarCipher(void * args);
void *changeCase(void * args);
void *nextQueue1(void *args);
void *nextQueue2(void *args);
void *sendNode(void *args);
void *nodeCounter(void * args);

QNode * newQNode(void *data)
{
    QNode *temp = (QNode *)malloc(sizeof(QNode));
    temp->data = (void*)malloc(sizeof(data));
    strcpy(temp->data,data);
    temp->next = NULL;
    return temp;
}

Queue * createQ()
{
    Queue *temp = (Queue *)malloc(sizeof(Queue));
    temp->front = temp->rear = NULL;
    return temp;
}

// Function to remove
// a key from given queue q
void *deQ(Queue *q)
{
    pthread_mutex_lock(&lock1);
    // If queue is empty, return NULL.
    if (q->front == NULL){
        printf("Waiting for condition lock\n");
        pthread_cond_wait(&cond, &lock1);
    }

    // Store previous front and
    // move front one node ahead
    QNode *temp = q->front;
    if (q->front)
    {
        q->front = q->front->next;
    }

    // If front becomes NULL, then
    // change rear also as NULL
    if (q->front == NULL){
        q->rear = NULL;
    }
    pthread_mutex_unlock(&lock1);
    return temp;
}

void enQ(Queue *q, void *x)
{
    pthread_mutex_lock(&lock1);
    // Create a new LL node
    QNode *new = newQNode(x);

    // If queue is empty, then
    // new node is front and rear both
    if (q->rear == NULL)
    {
        q->front = q->rear = new;
        pthread_cond_signal(&cond);
    }
    else {
        // Add the new node at
        // the rear of queue and change rear
        q->rear->next = new;
        q->rear = new;
    }
    pthread_mutex_unlock(&lock1);  
}

void destroyQ(Queue *q)
{
    while (q->front != NULL)
    {
        deQ(q);
    }
    free(q);
}

void newAO(Queue *q, void *(*qFun)(void *), void *(*endFun)(void *)) {
    for (;;){
        QNode * temp = (QNode *)deQ(q);
        qFun(temp);
        endFun(temp);
    }
}

void * newAO2(void * args){
    AO * ao = (AO *)args;
    newAO(ao->q, ao->qFun, ao->endFun);
    return NULL;
}

void destroyAO(AO * ao) {
    destroyQ(ao->q);
    pthread_cancel(ao->t_id);
    free(ao);
}

//ceasar cipher with shift of 1
void *caesarCipher(void * args){
    if (!args){
        return NULL;
    }
    QNode * temp = (QNode *)args;
    char * str = malloc(sizeof(temp->data));
    str = temp->data;
    for(int i = 0; i < strlen(temp->data); i++){
        if(str[i] >= 'a' && str[i] <= 'z'){
            str[i] = (str[i] - 'a' + 1) % 26 + 'a';
        }
        else if(str[i] >= 'A' && str[i] <= 'Z'){
            str[i] = (str[i] - 'A' + 1) % 26 + 'A';
        }
    }
    temp->data = str;
    return NULL;
}

//change each char to lowercase or uppercase
void *changeCase(void * args){
    if (!args){
        return NULL;
    }
    QNode * temp = (QNode *)args;
    char * str = malloc(sizeof(temp->data));
    str = temp->data;
    for(int i = 0; i < strlen(temp->data); i++){
        if(str[i] >= 'a' && str[i] <= 'z'){
            str[i] = str[i] - 'a' + 'A';
        }
        else if(str[i] >= 'A' && str[i] <= 'Z'){
            str[i] = str[i] - 'A' + 'a';
        }
    }
    temp->data = str;
    return NULL;
}

void *nextQueue1(void *args)
{
    if (!args)
    {
        return NULL;
    }
    struct QNode *temp = (struct QNode*) args;
    int len = strlen(temp->data);
    char str[len];
    strcpy(str,temp->data);
    enQ(q2,str);
    return NULL;
}

void *nextQueue2(void *args)
{
    if (!args)
    {
        return NULL;
    }
    struct QNode *temp = (struct QNode*) args;
    int len = strlen(temp->data);
    char str[len];
    strcpy(str,temp->data);
    enQ(q3,str);
    return NULL;
}

void *sendNode(void *args){
    if (!args)
    {
        return NULL;
    }
    struct QNode *temp = (struct QNode*) args;
    char str[strlen(temp->data)];
    strcpy(str,temp->data);
    if (send(sock, str, strlen(str), 0) == -1){
        perror("send");
    }
    return NULL;
}

void *nodeCounter(void * args){
    if (!args)
    {
        return NULL;
    }
    counter++;
    printf("Node number: %d reached the end\n", counter);
    return NULL;
}