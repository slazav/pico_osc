Name:         pico_rec
Version:      2.1
Release:      alt1

%define libname SigLoad
%define libver  1.0

Summary:      Record signals with Picoscope PS4224 and process them.
Group:        System
URL:          https://github.com/slazav/pico_osc
License:      GPL

Packager:     Vladislav Zavjalov <slazav@altlinux.org>

Source:       %name-%version.tar
BuildRequires: libfftw3-devel libjansson-devel libpng-devel tcl-blt-devel
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
* Thu May 02 2019 Vladislav Zavjalov <slazav@altlinux.org> 2.1-alt1
- Version of May 2019 left in LTL.
- pico_rec:
  - fix SPP interface
  - signal averaging
  - fix t0abs parameter handling, millisecond precision
  - new commands:
    - *idn? -- write id string
    - avr_start -- start averaging
    - avr_stop  -- stop averaing
    - avr_save <file> -- save average buffer
    - filter <file> <args> -- run sig_filter program
    - get_time -- print current time (unix seconds with ms precision)
    - chan_get <ch>  -- get channel parameters
    - trig_get  -- get trigger parameters
  - multiple channels can be set by chan_set command
  - do not write file in block command if name is -
- sig_filter:
  - SIGF format support (filtered signals in frequency domain)
  - WAV format support, sig2wav tool
  - change interface, allow independent filter options
  - fix range check for time crop
  - new filters:
    - sfft_peaks -- sliding fft with Blackman window, peak detection
    - sfft_peak  -- sliding fft with Blackman window, detect peak near some line
    - sfft_int   -- sliding fft with Blackman window, integral
    - sfft_diff  -- sliding fft with Blackman window, difference
    - dc         -- print mean value for each channel
    - minmax     -- print min/max values for each channel
    - overload   -- print overload flag for each channel
    - sigf       -- write SIGF file (fft with frequency filtering)
    - sig        -- write SIG file
    - wav        -- write WAV file
    - fitn       -- Fit N fork signals (sort by frequency)
    - slockin    -- sliding lock-in
- fix building, fix gcc warnings
- add more sample siignals

* Sun Apr 30 2017 Vladislav Zavjalov <slazav@altlinux.org> 2.0-alt1
- v2.0
- pico_rec: interface changes
- pico_filter -> sig_filter: interface changed, correct fit of harmonic signals
- add sig_viewer
- signal format changed

* Sun Dec 11 2016 Vladislav Zavjalov <slazav@altlinux.org> 1.0-alt1
- v1.0

