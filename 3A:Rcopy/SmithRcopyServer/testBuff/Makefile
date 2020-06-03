# udpCode makefile
# written by Hugh Smith - April 2017

CC = gcc
CFLAGS= -g -Wall
LIBS += libcpe464.2.20.a -lstdc++ -ldl
SRC = networks.c  networks.h gethostbyname.c gethostbyname.h checksum.c checksum.h srej.c srej.h buffer.c buffer.h
OBJS = networks.o gethostbyname.o checksum.o srej.o buffer.o

all:  buffer

buffer: buffer.c $(OBJS)
	$(CC) $(CFLAGS) -o buffer $(OBJS) $(LIBS)

.c.o: $SRC
	gcc -c $(CFLAGS) $< -o $@ 

cleano:
	rm -f *.o

clean:
	rm -f server buffer *.o

