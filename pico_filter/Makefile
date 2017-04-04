LDLIBS = -lm -lfftw3
CPPFLAGS = -g
CC=g++

all: pico_filter testsig

f1=../../signals/fork1.dat
f2=../../signals/fork2.dat

test: testsig pico_filter
	rm -f tests/ex1ch.dat tests/ex2ch.dat
	ln -s ${f1} tests/ex1ch.dat ||:
	ln -s ${f2} tests/ex2ch.dat ||:
	./testsig -N 100000 -D 1e-5 > tests/test1.dat
	./testsig -N 100000 -D 1e-5 -G 1000 -U 0.12> tests/test2.dat
	for i in ex1ch ex2ch test1 test2; do \
	  ./pico_filter -f txt     tests/$$i.dat > tests/$$i.txt; \
	  ./pico_filter -f pnm     tests/$$i.dat | pnmtopng > tests/$$i.png; \
	  ./pico_filter -f fft_txt -F32000 -G34500 tests/$$i.dat > tests/$${i}_fft.txt; \
	  ./pico_filter -f sfft_txt -F32000 -G34500 -T 3.05e-3 -w 4096 tests/$$i.dat > tests/$${i}_sfft.txt; \
	  ./pico_filter -f sfft_pnm -F32000 -G34500 -T 3.05e-3 -w 4096 tests/$$i.dat | pnmtopng > tests/$${i}_sfft.png; \
	  ./pico_filter -f sfft_pnm_ad -F32000 -G34500 tests/$$i.dat | pnmtopng > tests/$${i}_sfft_ad.png; \
	  ./pico_filter -f fit     -T 3.05e-3 -F32000 -G34500 tests/$$i.dat > tests/$${i}_fork.txt; \
	  #./pico_filter -f taf      -T 0 -F32000 -G34500 tests/$$i.dat > tests/$${i}_taf.txt; \
	done

#	./pico_filter -f pnm test.dat | pnmtopng > test_raw.png
#	./pico_filter -f sfft_txt test.dat > test_sfft.txt


pico_filter: pico_filter.o data.o signal.o rainbow.o fit_signal.o
testsig: testsig.o


pico_filter.o:\
   ../pico_rec/err.h data.h pico_filter.cpp
data.o:\
   ../pico_rec/err.h dimage.h signal.h fft.h data.h data.cpp
rainbow.o: rainbow.h

testsig.o:\
   ../pico_rec/err.h testsig.cpp

clean:
	rm -f *.o pico_filter testsig



