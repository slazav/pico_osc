Name:         pico_rec
Version:      2.0
Release:      alt1

%define libname SigLoad
%define libver  1.0

Summary:      Record signals with Picoscope PS4224 and process them.
Group:        System
URL:          https://github.com/slazav/pico_osc
License:      GPL

Packager:     Vladislav Zavjalov <slazav@altlinux.org>

Source:       %name-%version.tar
BuildRequires: libfftw3-devel
Requires:      libfftw3 tcl-xblt

%description
pico_rec - record signals with Picoscope PS4224 and process them

%prep
%setup -q

%build
%make

%install
%makeinstall

%files
%_bindir/*
%_tcllibdir/sig_load.so
%dir %_tcldatadir/%libname-%libver
%_tcldatadir/%libname-%libver/pkgIndex.tcl

%changelog
* Sun Apr 30 2017 Vladislav Zavjalov <slazav@altlinux.org> 1.1-alt1
- v2.0
- pico_rec: interface changes
- pico_filter -> sig_filter: interface changed, correct fit of harmonic signals
- add sig_viewer
- signal format changed

* Sun Dec 11 2016 Vladislav Zavjalov <slazav@altlinux.org> 1.0-alt1
- v1.0

