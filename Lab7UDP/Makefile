# udpCode makefile
# written by Hugh Smith - April 2017

CC = gcc
CFLAGS= -g -Wall
LIBS += libcpe464.2.20.a -lstdc++ -ldl
SRC = networks.c  networks.h gethostbyname.c gethostbyname.h packet.c packet.h checksum.c checksum.h
OBJS = networks.o gethostbyname.o packet.o checksum.o

all:  udpClient udpServer

udpClient: udpClient.c $(OBJS)
	$(CC) $(CFLAGS) -o udpClient udpClient.c $(OBJS) $(LIBS)

udpServer: udpServer.c $(OBJS)
	$(CC) $(CFLAGS) -o udpServer udpServer.c $(OBJS) $(LIBS)
	
.c.o: $SRC
	gcc -c $(CFLAGS) $< -o $@ 

cleano:
	rm -f *.o

clean:
	rm -f udpServer udpClient *.o

