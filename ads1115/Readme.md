Interface to ADS1113/1114/1115 ADC converters (i2c connection)

#### Command line options:

*  `-d <dev>`  -- I2C device path (default /dev/i2c-0)
*  `-a <addr>` -- I2C address (default 0x48)
*  `-c <chan>` -- change default channel setting (default AB)
*  `-v <chan>` -- change default range setting (default 2.048)
*  `-r <chan>` -- change default rate setting (default 8)
*  `-m <mode>` -- program mode: spp, single, cont (default: spp)
*  `-d <time>` -- delay in cont mode [s] (default: 1.0)
*  `-h`        -- write this help message and exit

Channel settings: A, B, C, D, AB, AD, BD, CD.

Range settings: 6.144, 4.096, 2.048, 1.024, 0.512, 0.256 [V].
Bi-directional in differential mode (AB, CD, etc).

Rate settings: 8, 16, 32, 64, 128, 250, 475, 860 [1/s].

#### Program can work in a few regimes:


1. SPP mode (`-m spp`)

Implements SPP interface for using it with device server.

Example: print device configuration; do a measurement of channel A at
range 1.024V and rate 1/16s; do a measurement with default settings
(channel AB, range 2.048, rate 1/8s). Default settings can be changed
with -c, -r, -v option.

```
$ ./ads1115 -d /dev/i2c-3
#SPP001
Using /dev/i2c-3:0x48 as a ADS1113/1114/1115 device.
Type help to see command list.
#OK
*idn?
ads1115 1.0
#OK
get_conf
conversion register: 0x2070
lo_thresh register: 0x8000
hi_thresh register: 0x7fff
config register:    0xc7a3
status:     ready
chan:       A
range:      1.024 V
mode:       single
rate:       1/250 s
comp.type:  window
comp.pol:   act.high
comp.latch: on
comp.queue: off
#OK
get A 1.024 16
0.257937
#OK
get
0.000000
#OK
```


2. Single measurement mode (`-m single`)

Do a single measurement with defult settings, print result and exit.

Example:
```
$ ./ads1115 -d /dev/i2c-3 -m single
0.000000
$ ./ads1115 -d /dev/i2c-3 -m single -c D
0.268188
```


3. Info mode (`-m info`)

Print device information (all registers) and exit.


Example:
```
$ ./ads1115 -d /dev/i2c-3 -m info
conversion register: 0x10c0
lo_thresh register: 0x8000
hi_thresh register: 0x7fff
config register:    0xe503
status:     ready
chan:       C
range:      2.048 V
mode:       single
rate:       1/8 s
comp.type:  window
comp.pol:   act.high
comp.latch: on
comp.queue: off

```


4. Continuous mode (`-m cont`)

Do measurements in a loop, print results with timestamps.
Delay between measurements (in seconds) can be set with `-t` option.

Example (measure channal A with conversion time 1/8s, without any delay between measurements):
```
# ./ads1115 -d /dev/i2c-3 -t0 -r8 -m cont -c A
1612617919.027378       0.268062
1612617919.156301       0.268125
1612617919.285006       0.268250
1612617919.413711       0.268062
1612617919.542406       0.268188
1612617919.671107       0.268250
1612617919.799795       0.268125
1612617919.928509       0.268250
1612617920.057207       0.268125
1612617920.185898       0.268250
```
