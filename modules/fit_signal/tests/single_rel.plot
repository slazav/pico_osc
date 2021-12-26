#!/usr/bin/gnuplot

fname="single_rel.dat"

set nokey
set xlabel "noise/amp ratio"

set title "Frequency, relative error"
plot  \
 fname using 2:3 pt 7,\
 4e-7*x lc 0, -4e-7*x lc 0

pause -1

set title "Relaxartion, relative error"
plot  \
 fname using 2:4 pt 7,\
 0.06*x lc 0, -0.06*x lc 0

pause -1


set title "Amplitude, relative error"
plot  \
 fname using 2:5 pt 7,\
 0.015*x lc 0, -0.015*x lc 0

pause -1

set title "Phase error"
plot  \
 fname using 2:6 pt 7,\
 0.015*x lc 0, -0.015*x lc 0

pause -1
