all:
	make -C pico_osc
	make -C pico_adc
	make -C sig_filter
	make -C sig_viewer
	make -C fit_res

dir=/usr$(subst ${prefix},,${libdir})/tcl

name=SigLoad
ver=1.0

bindir ?= /usr/bin

install: all
	mkdir -p ${bindir} ${libdir}/tcl/ ${datadir}/tcl/${name}-${ver}
	install -m755 pico_osc/pico_osc pico_adc/pico_adc ${bindir}
	install -m755 sig_filter/{sig_filter,sig_pnmtopng,sig_pnginfo} ${bindir}
	install -m755 fit_res/fit_res ${bindir}
	install -m644 sig_viewer/sig_load.so ${libdir}/tcl/
	sed 's|%LIB_DIR%|${dir}|' sig_viewer/pkgIndex.tcl > ${datadir}/tcl/${name}-${ver}/pkgIndex.tcl
	sed 's|^load ./sig_load.so|package require SigLoad|' sig_viewer/sig_viewer > ${bindir}/sig_viewer
	chmod 755 ${bindir}/sig_viewer

#



