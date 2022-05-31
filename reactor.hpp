#pragma once

#include <iostream>
#include <pthread.h>

typedef struct reactor
{
    int fd;
    pthread_t t_id;
    void*(*fun)(void*);
} reactor;

typedef struct request{
    int fd;
    reactor * reac;
} reqeust;

reactor * newReactor();
void InstallHandler(reactor * reac, void*(fun)(void*), int fd);
void RemoveHandler(reactor * reac, int fd);