LDLIBS = -lm -lfftw3
CPPFLAGS = -g
CC=g++

all: test_signal
	./test_signal ../../pico_rec/ex1ch.txt sig1.dat
	./test_signal ../../pico_rec/ex2ch.txt sig2.dat
	./test_signal sig1.dat sig1a.dat
	./test_signal sig2.dat sig2a.dat
	diff sig1.dat sig1a.dat
	diff sig2.dat sig2a.dat
#	rm -f sig1.dat sig1a.dat sig2.dat sig2a.dat

test_signal: test_signal.cpp ../signal.cpp

clean:
	rm -f *.o test_signal



