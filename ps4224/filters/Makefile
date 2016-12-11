LDLIBS = -lm -lfftw3
CPPFLAGS = -g
CC=g++

test: pico_filter testsig
	./testsig > test.dat
	./pico_filter -f txt test.dat > test.txt
	./pico_filter -f pnm test.dat | pnmtopng > test_raw.png
	./pico_filter -f sfft_txt test.dat > test_sfft.txt


pico_filter: pico_filter.o data.o
testsig: testsig.o


pico_filter.o:\
   ../err.h data.h pico_filter.cpp
data.o:\
   ../err.h data.h data.cpp

testsig.o:\
   ../err.h testsig.cpp

clean:
	rm -f *.o pico_filter testsig



