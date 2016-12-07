LIB=ps4000

CFLAGS=-I/usr/include/libps -g -D$(LIB)
LDFLAGS=-L/opt/picoscope/lib
LDLIBS=-lpicoipp -l$(LIB) -lmicrohttpd -lm -lfftw3

all: pico_server

OBJ=pico_server.o commands.o convs.o rec.o

pico_server: $(OBJ)
$(OBJ): pars.def pico.h

clean:
	rm -f *.o pico_server