# udpCode makefile
# written by Hugh Smith - April 2017

CC = gcc
CFLAGS= -g -Wall
LIBS += libcpe464.2.20.a -lstdc++ -ldl
SRC = networks.c  networks.h gethostbyname.c gethostbyname.h checksum.c checksum.h srej.c srej.h args.h args.c buffer.h buffer.c
OBJS = networks.o gethostbyname.o checksum.o srej.o args.o buffer.o

all:  rcopy server

rcopy: rcopy.c $(OBJS)
	$(CC) $(CFLAGS) -o rcopy rcopy.c $(OBJS) $(LIBS)

server: server.c $(OBJS)
	$(CC) $(CFLAGS) -o server server.c $(OBJS) $(LIBS)
	
.c.o: $SRC
	gcc -c $(CFLAGS) $< -o $@ 

cleano:
	rm -f *.o

clean:
	rm -f server rcopy *.o

