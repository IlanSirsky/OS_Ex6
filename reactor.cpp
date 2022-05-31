#include "reactor.hpp"

reactor * newReactor(){
    reactor * reac = (reactor *)(malloc(sizeof(reactor)));
    return reac;
}

void InstallHandler(reactor * reac, void*(fun)(void*), int fd){
    reac->fun = fun;
    reac->fd = fd;
    reqeust * reqests = (reqeust *)(malloc(sizeof(reqeust)));
    reqests->fd = fd;
    reqests->reac = reac;
    pthread_create(&reac->t_id, NULL, fun, reqests);
}

void RemoveHandler(reactor * reac, int fd){
    pthread_join(reac->t_id, NULL);
    reac->fd = -1;
    reac->fun = NULL;
}