LDLIBS = -lm -lps4000 -lfftw3
CC=g++

all: pico4224 unpack_data fft_filter

pico4224: pico4224.o pico_err.o m_getopt.o

pico4224.o:\
   err.h buf.h pico_4224.h pico_err.h pico_int.h pico_cmd.h
