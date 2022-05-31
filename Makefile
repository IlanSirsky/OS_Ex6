CPP=g++
CC=gcc
AR=ar
FLAGS= -Wall -g3

all: main1 client guard singleton pollserver #reactorclient 

main1: main1.o mainQueue.h
	$(CC) $(FLAGS) -o main1 main1.o -lpthread

guard: guard.o
	$(CPP) $(FLAGS) -o guard guard.o -lpthread

singleton: singleton.o
	$(CPP) $(FLAGS) -o singleton singleton.o

pollserver: pollserver.o 
	$(CPP) $(FLAGS) pollserver.cpp reactor.cpp -o pollserver -pthread -lpthread 

clientReactor: clientReactor.o 
	$(CC) $(FLAGS) clientReactor.c -o clientReactor -pthread

client: client.o
	$(CPP) $(FLAGS) client.cpp -o client -lpthread

main1.o: main1.c
	$(CC) $(FLAGS) -c main1.c

mainQueue.o: mainQueue.c
	$(CC) $(FLAGS) -c mainQueue.c

guard.o: guard.cpp
	$(CPP) $(FLAGS) -c guard.cpp

singleton.o: singleton.cpp
	$(CPP) $(FLAGS) -c singleton.cpp

pollserver.o: pollserver.cpp
	$(CPP) $(FLAGS) -c pollserver.cpp

clientReactor.o: clientReactor.c
	$(CC) $(FLAGS) -c clientReactor.c

client.o: client.cpp
	$(CPP) $(FLAGS) -c client.cpp

.PHONY: clean all
clean:
	rm -f *.o main1 guard singleton pollserver reactorclient client