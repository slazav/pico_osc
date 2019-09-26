PROGRAMS = pico_osc pico_adc


MOD_HEADERS := pico_4224.h pico_adc24.h pico_err.h picoosc_int.h picoadc_int.h
MOD_SOURCES := pico_err.cpp picoosc_int.cpp picoadc_int.cpp

LDLIBS += -lm -lps4000 -lpicohrdl -lfftw3

MODDIR = ../modules
include $(MODDIR)/Makefile.inc

