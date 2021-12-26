#!/usr/bin/gnuplot

fname="double_rel.dat"

set nokey
set xlabel "freq"

set title "Frequency error"
plot  \
 fname using 1:($3-$1) pt 7,\
 0 lc 0

pause -1

set title "Relaxartion"
plot  \
 fname using 1:4 pt 7,\
 0 lc 0

pause -1


set title "Amplitude"
plot  \
 fname using 1:5 pt 7,\
 0.582 lc 0

pause -1

set title "Phase"
plot  \
 fname using 1:6 pt 7,\
 0.1234 lc 0

pause -1
