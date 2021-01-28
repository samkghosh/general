


CC=g++
AR=ar -rcs 
LIB=../lib/libdb.a
//CFLAGS=-c -D TESTOUT
INC=../inc

MYSQLINC=/usr/include/mysql

CFLAGS=-c -I$(MYSQLINC) -I$(INC)

OBJS=database.o

all: $(OBJS)
	$(AR) -o $(LIB)  $(OBJS)

.c.o:
	$(CC) -c -I(INC) $< 2>>err

.cpp.o:
	$(CC) $(CFLAGS) -I$(INC) $< 2>>err



clean:
	rm -rf *.o
