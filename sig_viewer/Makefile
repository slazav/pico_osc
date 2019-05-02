LDLIBS += -lBLT -ltk -lfftw3
CXXFLAGS += -shared -O9 -fPIC

ALL_SOURCES = sig_load.cpp

MODDIR = ../modules
include $(MODDIR)/Makefile.inc

make_nodeps: sig_load.so
sig_load.so: sig_load.o $(ADEPS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ ${LDLIBS} -o $@

clean:
	rm -f *.o *.so