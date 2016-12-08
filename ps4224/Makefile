LDLIBS = -lm -lps4000
CC=g++

all: test1 test2 ps4224block

test1: test1.o convs.o
test2: test2.o convs.o
ps4224block: ps4224block.o convs.o m_getopt.o

ps4224block.o test1.o test2.o: test1.cpp err.h ps4224.h buf.h convs.h
