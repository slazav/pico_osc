LDLIBS = -lm -lps4000
CC=g++

all: test1 test2

test1: test1.o convs.o
test2: test2.o convs.o

test1.o: test1.cpp err.h ps4224.h buf.h convs.h
test2.o: test1.cpp err.h ps4224.h buf.h convs.h