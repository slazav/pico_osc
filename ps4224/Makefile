LDLIBS = -lm -lps4000 -lfftw3
CPPFLAGS = -g
CC=g++

all: pico_rec

pico_rec: pico_rec.o pico_err.o pico_int.o

pico_rec.o:\
   err.h pico_4224.h pico_err.h pico_int.h

clean:
	rm -f *.o pico_rec unpack_data fft_filter