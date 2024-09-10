all:
	make -C pico_osc
	make -C pico_adc

#dir=/usr$(subst ${prefix},,${libdir})/tcl

DESTDIR    ?=
prefix     ?= $(DESTDIR)/usr
bindir     ?= $(DESTDIR)/usr/bin
datadir    ?= $(DESTDIR)/usr/share
libdir     ?= $(DESTDIR)/usr/lib64
sysconfdir ?= $(DESTDIR)/etc
initdir    ?= $(DESTDIR)/etc/init.d
tcldatadir ?= $(DESTDIR)/usr/share/tcl
man1dir    ?= $(DESTDIR)/usr/share/man/man1
srvdir     ?= $(DESTDIR)/usr/lib/systemd/system

bindir ?= /usr/bin

install: all
	install -m755 pico_osc/pico_osc pico_adc/pico_adc ${bindir}

#



