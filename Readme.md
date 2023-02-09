## pico_osc -- programs for recording and processing oscilloscope signals

### Folders:

* pico_rec -- SPP interface to Picoscope 4000 devices. (SPP is a "simple pipe protocol" used
to communicate with command-line programs via unix pipes. It is used in some of
my programs: graphene, device2. This allows to use the oscilloscopes in the device2 system,
but the communication can be also done with simple shell scripts).
Program res

* pico_log_py -- Python library for accessing oscilloscopes (by M.Will).

* pico_adc -- SPP interface to Pico ADC24 devices.

* ads1115 -- Communication with ads1115 ADC cards. I'm using at my Raspberry Pi.
There is an SPP/command line interface program (ads1115.cpp), but it is not well tested,
because I'm using more specialized mesurement program (pmeas.cpp).

* sig_filter  -- Program for processing signals recorded by pico_rec program.
I'm using a simple custom format SIG for recording signals. It contains a
text header and binary data from oscilloscope. There is also SIGF
variant written in frequency domain (it is possible to reduce frequency
range and make files a lot smaller). Sig_filter program
does a lot of operations with signals.

* sig_pngfig  -- Working with PNG spectrograms with fig wrapper. It is possible
to convert sig file to a png spectrogram with additional information
about original time and frequency range; put it in a fig file (for xfig
vector editor), manually mark some features and then use sig_filter to extract
data from sig files using these marks.

* sig_viewer  -- TCL viewer for signal (and text) files

* sig_python  -- a simple python library for reading sig files

* signals     -- signal examples

* ps3000      -- old programs for ps3000 device: HTTP server + tcl interface

* pico_repack -- scripts for repacking pico libraries from original DEBs
to Altlinux RPMs.

* modules -- I'm using my mapsoft2 build system with local set of modules.
See https://github.com/slazav/mapsoft2-libs
