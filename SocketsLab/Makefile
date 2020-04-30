# Makefile for CPE464 tcp test code
# written by Hugh Smith - April 2019

CC= gcc
CFLAGS= -g -Wall
LIBS = 


all:   sClient sServer

sClient: sClient.c networks.o gethostbyname6.o
	$(CC) $(CFLAGS) -o cclient sClient.c networks.o gethostbyname6.o $(LIBS)

sServer: sServer.c networks.o gethostbyname6.o
	$(CC) $(CFLAGS) -o server sServer.c networks.o gethostbyname6.o $(LIBS)

.c.o:
	gcc -c $(CFLAGS) $< -o $@ $(LIBS)

cleano:
	rm -f *.o

clean:
	rm -f cclient server sServer sClient *.o




