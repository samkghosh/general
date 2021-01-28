

#Makefile to compile log library Library
CC=g++ 
AR=ar -rcs 
LIB=../lib/libeasy.a
//CFLAGS=-c -D TESTOUT
CFLAGS=-c
INC=../inc

OBJS= calccrc.o  md5.o md5hash.o easy.o

#apendtxt.o \
#checkval.o \
#chkpath.o \
#chksettm.o \
#cleanupFile.o \
#convtm.o \
#copyfile.o \
#cregproc.o \
#delfiles.o \
#egCrypt.o \
#findtm.o \
#getfiles.o \
#ping.o \
#serialport.o \
#slock.o \
strip.o 

#strtok.o \
#waveutil.o


all: $(OBJS)
	$(AR) -o $(LIB)  $(OBJS)

.c.o:
	$(CC) -c -I$(INC) $< 2>>err

.cpp.o:
	$(CC) $(CFLAGS) -I$(INC) $< 
#2>>err

clean:
	rm -rf *.o




