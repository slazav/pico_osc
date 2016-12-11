LDLIBS = -lm -lfftw3
CPPFLAGS = -g
CC=g++

test: pico_filter testsig
	ln -s ../../ex1ch.txt tests/ex1ch.dat ||:
	ln -s ../../ex2ch.txt tests/ex2ch.dat ||:
	./testsig -N 100000 -D 1e-5 > tests/test1.dat
	./testsig -N 100000 -D 1e-5 -G 1000 -U 0.12> tests/test2.dat
	for i in ex1ch ex2ch test1 test2; do \
	  ./pico_filter -f txt     tests/$$i.dat > tests/$$i.txt; \
	  ./pico_filter -f pnm     tests/$$i.dat | pnmtopng > tests/$$i.png; \
	  ./pico_filter -f fft_txt -F32000 -G34500 tests/$$i.dat > tests/$${i}_fft.txt; \
	  ./pico_filter -f sfft_txt -F32000 -G34500 -T 3.05e-3 -w 4096 tests/$$i.dat > tests/$${i}_sfft.txt; \
	  ./pico_filter -f sfft_pnm -F32000 -G34500 -T 3.05e-3 -w 4096 tests/$$i.dat | pnmtopng > tests/$${i}_sfft.png; \
	  ./pico_filter -f sfft_pnm_ad -F32000 -G34500 tests/$$i.dat | pnmtopng > tests/$${i}_sfft_ad.png; \
	  ./pico_filter -f fork     -F32000 -G34500 tests/$$i.dat > tests/$${i}_fork.txt; \
	done

#	./pico_filter -f pnm test.dat | pnmtopng > test_raw.png
#	./pico_filter -f sfft_txt test.dat > test_sfft.txt


pico_filter: pico_filter.o data.o rainbow.o
testsig: testsig.o


pico_filter.o:\
   ../err.h data.h pico_filter.cpp
data.o:\
   ../err.h data.h data.cpp
rainbow.o: rainbow.h

testsig.o:\
   ../err.h testsig.cpp

clean:
	rm -f *.o pico_filter testsig



