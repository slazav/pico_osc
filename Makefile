all:
	make -C ps4224

install: all
	mkdir -p ${bindir}
	install ps4224/pico_rec ps4224/filters/pico_filter ${bindir}
