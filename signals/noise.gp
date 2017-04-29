#!/usr/bin/gnuplot

set log
set title "oscilloscope noise"
set xlabel "Hz"
set xlabel "nV/sqrt(Hz)"

plot\
 "noise.txt" using 1:(1e9*sqrt($2)) with lines,\
 "noise1.txt" using 1:(1e9*sqrt($2)) with lines
pause -1