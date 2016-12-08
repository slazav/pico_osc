LDLIBS = -lm -lps4000
CC=g++

all: pico4224block pico4224stream unpack_data

pico4224block: pico4224block.o pico_err.o m_getopt.o
pico4224stream: pico4224stream.o pico_err.o m_getopt.o

pico4224stream.o pico4224block.o:\
   err.h buf.h pico_4224.h pico_err.h pico_int.h pico_cmd.h
