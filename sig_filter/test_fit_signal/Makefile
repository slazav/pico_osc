LDLIBS = -lm -lfftw3
CPPFLAGS = -g
CC=g++

all: test_fit_signal
	./test_fit_signal > res.txt
	./plot_res.gp

test_fit_signal: test_fit_signal.cpp ../fit_signal.cpp

clean:
	rm -f *.o test_fit_signal



