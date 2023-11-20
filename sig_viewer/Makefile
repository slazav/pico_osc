SHARED_LIB := sig_load
MOD_SOURCES := sig_load.cpp

PKG_CONFIG += tcl

LDLIBS += -lBLT -ltk -lfftw3 -ltcl
CXXFLAGS = -I/usr/include/tcl

MODDIR = ../modules
include $(MODDIR)/Makefile.inc

