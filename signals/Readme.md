## Signal format

File starts with "*SIG001" line which shows the format name and version
Then header goes followed by a "*" line.
Header can contain comments (started with #), empty lines
and "<key>: <value1> <value2> ..." lines.

The most important keys are:
* dt -- time distance between points, s (default: 1)
* t0 -- relative time of the first point, s (default: 0)
* t0abs -- system time of t=0 position, s (default: 0)
* chan -- channel specification, contains three values:
   * 1-symbol name (A, B, ...)
   * y-scale
   * overflow flag

After the header and the "*" line there is data. It is an array
of points. Each point contains 2-byte integers, one for each channel.

TODO: packing


## Examples of signals

============================================================
noise.dat -- oscilloscope voltage noise. input is shortened to ground.

    Processing:

# pico_filter -f fft_pow_lavr noise.dat > noise.txt

(gives 1024-points spectrum with noise power in V^2/Hz,
see noise.gp plotting script)

============================================================
fork1.dat, fork2.dat -- quartz tuning fork pulse response
fork2 contains second channel with generator sync signal.
Trigger position is t=0, end of pulse is t=0.0032

    Processing:

# pico_filter -f pnm fork2.dat | pnmtopng > fork2.png
(gives a picture with both channels)

# pico_filter -f txt -T -0.001 -U 0.004 fork2.dat > fork2.txt
(gives a text table with selected time range)

# pico_filter -f fit -T 0.0032 fork1.dat
1481432998 32764.4807367 1.20337 0.0486501 -3.36399
(absolute time, frequency, 1/tau, amplitude, phase)

============================================================
lockin1.dat -- 10mV, 25kHz signal + generator sync

    Processing:

# pico_filter -f lockin lockin1.dat
25000.048189 0.0101066 9.95048e-05
(frequency, amplitude, phase)

============================================================
corr10k.dat, corr_nc -- 2-channal noise signals for
correlation measurements: non correlated 50R and correlated 10k.
