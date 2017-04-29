all:
	make -C pico_rec
	make -C sig_filter

install: all
	mkdir -p ${bindir}
	install pico_rec/pico_rec sig_filter/sig_filter ${bindir}
