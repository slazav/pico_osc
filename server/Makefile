CFLAGS=-I/opt/picoscope/include/libps3000a-1.1 -g
LDFLAGS=-L/opt/picoscope/lib
LDLIBS=-lpicoipp -lps3000a -lmicrohttpd -lm

all: pico_server

OBJ=pico_server.o commands.o convs.o

pico_server: $(OBJ)
$(OBJ): pars.def pico.h

clean:
	rm -f *.o pico_server