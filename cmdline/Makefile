CFLAGS=-I/opt/picoscope/include/libps3000a-1.1 -g
LDFLAGS=-L/opt/picoscope/lib
LDLIBS=-lpicoipp -lps3000a

all: pico

pico: pico.o
pico.o: pico.c pico_opts.c pico_lst.c pico_cmd.c

clean:
	rm -f *.o pico *.log *.dat

