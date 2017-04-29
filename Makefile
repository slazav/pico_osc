all:
	make -C pico_rec
	make -C sig_filter
	make -C sig_viewer

install: all
	mkdir -p ${bindir} ${libdir}/tcl
	install pico_rec/pico_rec sig_filter/sig_filter ${bindir}
	sed "s|load ./sig_load.so|load ${libdir}/tcl/sig_load.so|" sig_viewer/sig_viewer > ${bindir}/sig_viewer
	install -m644 sig_viewer/sig_load.so ${libdir}/tcl/
