LIBS=`root-config --libs`
CFLAGS=`root-config --cflags`
CC=g++
COPT=-g
INCDIR= $(shell pwd)/include
SRCDIR= $(shell pwd)/src
OBJDIR= $(shell pwd)/obj
BINDIR= $(shell pwd)/bin

default:
	$(CC) $(COPT) timingrefC.cpp -o noise $(LIBS) $(DELPHLIBS) $(CFLAGS)

clean:
	rm noise *.o
