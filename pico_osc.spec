Name:         pico_rec
Version:      1.0
Release:      alt1

Summary:      Record signals with Picoscope PS4224 and process them.
Group:        System
URL:          https://github.com/slazav/pico_osc
License:      GPL

Packager:     Vladislav Zavjalov <slazav@altlinux.org>

Source:       %name-%version.tar
BuildRequires: libfftw3-devel
Requires:      libfftw3

%description
pico_rec - record signals with Picoscope PS4224 and process them

%prep
%setup -q

%build
%makeinstall

%files
%_bindir/*

%changelog
