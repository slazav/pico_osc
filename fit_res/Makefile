LDLIBS = -lgsl -lm

CC=g++

all: fit_res

fit_res: fit_res.o fit.o
fit_res.o: fit.h


clean:
	rm -f fit_res *.o



#test: fit_res_test
#	./fit_res_test > fit_res.dat
#	./plot
#
#fit_res_test: fit_res_test.o fit.o
#fit_res_test.o: fit.h
#
#
#split_sweeps: split_sweeps.o

