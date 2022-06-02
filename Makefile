CPP=g++
CC=gcc
AR=ar
FLAGS= -Wall -g3

all: main1 client guard singleton pollserver reactorclient 

main1: main1.o mainQueue.h
	$(CC) $(FLAGS) -o main1 main1.o -lpthread

client: client.o
	$(CPP) $(FLAGS) client.cpp -o client -lpthread

guard: guard.o
	$(CPP) $(FLAGS) -o guard guard.o -lpthread

singleton: singleton.o
	$(CPP) $(FLAGS) -o singleton singleton.o

pollserver: pollserver.o 
	$(CPP) $(FLAGS) pollserver.cpp reactor.cpp -o pollserver -pthread -lpthread 

reactorclient: reactorclient.o
	$(CC) $(FLAGS) reactorclient.o -o reactorclient -pthread

main1.o: main1.c
	$(CC) $(FLAGS) -c main1.c

client.o: client.cpp
	$(CPP) $(FLAGS) -c client.cpp

guard.o: guard.cpp
	$(CPP) $(FLAGS) -c guard.cpp

singleton.o: singleton.cpp
	$(CPP) $(FLAGS) -c singleton.cpp

pollserver.o: pollserver.cpp
	$(CPP) $(FLAGS) -c pollserver.cpp

reactorclient.o: reactorclient.c
	$(CC) $(FLAGS) -c reactorclient.c

# Didn't work (don't use) (multiple definition of main)
shared: main1.o guard.o singleton.o pollserver.o reactorclient.o client.o
	gcc --shared -fPIC -pthread -o libshared.so main1.o guard.o singleton.o pollserver.o reactorclient.o client.o -lm 

.PHONY: clean all
clean:
	rm -f *.o main1 guard singleton pollserver reactorclient client