CC = gcc
CFLAGS = -g -Wall 

all: clean trace

test: clean trace
	@ echo "Testing ./testing.sh"
	./testing.sh

trace: trace.o
	$(CC) $(CFLAGS) -o trace trace.o checksum.c  -lpcap 

trace.o:trace.c trace.h
	$(CC) $(CFLAGS) -g -c -o trace.o trace.c

clean:
	rm -f trace trace.o checksum.o
