#Makefile to compile Communication Library
CC=g++ 
AR=ar -rcs 
LIB=../lib/libcommlibs.a
//CFLAGS=-c -D TESTOUT
CFLAGS=-c
INC=../inc

OBJS= Tcpip.o Message.o utils.o 



all: $(OBJS)
	$(AR) -o $(LIB)  $(OBJS)

.c.o:
	$(CC) -c -I$(INC) $< 2>>err

.cpp.o:
	$(CC) $(CFLAGS) -I$(INC) $< 2>>err

clean:
	rm -rf *.o
