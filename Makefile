all:
	make -C spp-picoosc

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
	mkdir -p ${bindir}
	install -m755 spp-picoosc/pico_osc ${bindir}
