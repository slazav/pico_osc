LDLIBS = -lm -lps4000
CC=g++

all: test1 test2 pico4224block

test1: test1.o pico_err.o
test2: test2.o pico_err.o
pico4224block: pico4224block.o pico_err.o m_getopt.o

pico4224block.o test1.o test2.o: test1.cpp err.h buf.h pico_4224.h pico_err.h pico_int.h
