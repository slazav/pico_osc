LDLIBS=-lBLT -ltk
LDFLAGS=-shared -Wl,-rpath /usr/lib64/tcl
CPPFLAGS=-O9 -fPIC
CC=g++

all: sig_load.so

sig_load.so: sig_load.o ../pico_filter/signal.o
	${CC} -shared ${LDFLAGS} $^ ${LDLIBS} -o $@

sig_load.o: sig_load.cpp

clean:
	rm -f *.o *.so