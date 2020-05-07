# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall
LIBS = 


all:   cclient server

cclient: myClient.c test.o shared.o recvparse.o parse.o linkedlist.o networks.o pollLib.o gethostbyname6.o *.h
	$(CC) $(CFLAGS) -o cclient myClient.c test.o shared.o recvparse.o parse.o linkedlist.o networks.o pollLib.o gethostbyname6.o $(LIBS)

server: myServer.c test.o shared.o recvparse.o parse.o linkedlist.o networks.o pollLib.o gethostbyname6.o *.h
	$(CC) $(CFLAGS) -o server myServer.c test.o shared.o recvparse.o parse.o linkedlist.o networks.o pollLib.o gethostbyname6.o $(LIBS)

.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f myClient myServer server cclient *.o




