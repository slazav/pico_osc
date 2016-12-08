LDLIBS = -lm -lps4000
CC=g++

all: test1 test2 test3 pico4224block pico4224stream

pico4224block: pico4224block.o pico_err.o m_getopt.o
pico4224stream: pico4224stream.o pico_err.o m_getopt.o

pico4224stream.o pico4224block.o:\
   err.h buf.h pico_4224.h pico_err.h pico_int.h pico_cmd.h
