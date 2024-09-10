## pico_osc -- programs for recording and processing oscilloscope signals

### Folders:

* pico_rec -- SPP interface to Picoscope 4000 devices. (SPP is a "simple pipe protocol" used
to communicate with command-line programs via unix pipes. It is used in some of
my programs: graphene, device2. This allows to use the oscilloscopes in the device2 system,
but the communication can be also done with simple shell scripts).

* pico_log_py -- Python library for accessing oscilloscopes (by M.Will).

* pico_adc -- SPP interface to Pico ADC24 devices.

* ps3000      -- old programs for ps3000 device: HTTP server + tcl interface

* pico_repack -- scripts for repacking pico libraries from original DEBs
to Altlinux RPMs.

* modules -- I'm using my mapsoft2 build system with local set of modules.
See https://github.com/slazav/mapsoft2-libs
