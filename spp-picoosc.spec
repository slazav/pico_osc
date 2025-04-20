Name:         spp-picoosc
Version:      2.3
Release:      alt1

Summary:      SPP interface for Picoscope PS4224
Group:        System
URL:          https://github.com/slazav/spp-picoosc
License:      GPL

Packager:     Vladislav Zavjalov <slazav@altlinux.org>

Source:       %name-%version.tar

# old name
Provides: pico_osc
Obsoletes: pico_osc

%description
spp-picoosc - SPP interface for Picoscope PS4224

%prep
%setup -q

%build
%make

%install
%makeinstall

%files
%_bindir/*

%changelog
* Tue Oct 13 2020 Vladislav Zavjalov <slazav@altlinux.org> 2.3-alt1
changes in pico_adc:
 - rewrite code keeping old interface
 - modify get_info command: print more information
 - return +/-Inf at overflow
 - allow 1-digit channels in chan_set command
 - check timing configuration in get command
 - new command: set_dig_out, setting digital output
 - new command: disable_all, reset all channel settings
 - new command: get_val, read a single value, no channel setup needed
 - update documentation

* Sat Jul 25 2020 Vladislav Zavjalov <slazav@altlinux.org> 2.2-alt1
- start using Mapsoft2 module system (w/o git submodules)
- add pico_osc-9999.ebuild (by A.S.)
- add pico_adc program (mostly by A.S.)
- more test signals

- modules/fit_signal:
  - add fit_signal_fixfre function
  - better weighting function for fit
  - add test, add Readme file

- sig_filter
  - reading FLAC and GZ files
  - new filter: peak -- position of the main peak on FFT absolute value
  - new filter: sfft_pow -- print a text table with fft amplitude
  - sfft_pnm filter: -l option for log scale; -g option for choosing color gradient
  - sfft_pnm filter: set default window size to have same f and t resolution
  - -a option: average all channels (sfft_pnm, sfft_peaks filters)
  - -S option: width of a color scale (sfft_pnm, sfft_pnm_ad)
  - -B option: Blackman window (fft_txt, fft_pow, fft_pow_corr filters)
  - -P option: pulse detection
  - -M option: reading multiple signals
  - fix sigf reading
  - add some additional data to pnm files
  - sig_pnginfo and sig_pnmtopng programs (for using with sig_pngfig)

- pico_rec
 - filter command: check return value of system call
 - use read_words module for reading commands
 - fix filter command

- sig_pngfig: add script for wrapping png/pnm files in fig and extracting
   data semi-manually.

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

