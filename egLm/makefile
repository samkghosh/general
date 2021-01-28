
OBJS=crc.o egLm.o eglmsdk.o etheradd.o
CC=g++ 
AR=ar -rcs 
LIB=../lib/libegLm.a
//CFLAGS=-c -D TESTOUT
CFLAGS=-c
INC=../inc


all: $(OBJS)
	$(AR) -o $(LIB)  $(OBJS)

.c.o:
	$(CC) -c -I(INC) $< 2>>err

.cpp.o:
	$(CC) $(CFLAGS) -I$(INC) $< 2>>err
