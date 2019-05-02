PROGRAMS = pico_rec

LDLIBS += -lm -lps4000 -lfftw3

# additional sources:
ALL_SOURCES := pico_4224.h pico_err.h pico_int.h pico_err.cpp pico_int.cpp

MODDIR = ../modules
include $(MODDIR)/Makefile.inc

# additional dependencied
pico_rec: pico_rec.o pico_int.o pico_err.o
pico_rec.o pico_int.o pico_err.o: pico_4224.h pico_err.h pico_int.h

