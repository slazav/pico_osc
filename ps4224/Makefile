LDLIBS = -lm -lps4000 -lfftw3
CPPFLAGS = -g
CC=g++

all: pico4224int unpack_data fft_filter

pico4224int: pico4224int.o pico_err.o m_getopt.o pico_int.o

pico4224int.o:\
   err.h pico_4224.h pico_err.h pico_int.h

clean:
	rm -f *.o pico4224int unpack_data fft_filter