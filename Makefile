all:
	make -C pico_rec
	make -C pico_filter

install: all
	mkdir -p ${bindir}
	install pico_rec/pico_rec pico_filter/pico_filter ${bindir}
