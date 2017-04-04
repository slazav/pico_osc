#!/usr/bin/gnuplot

set xlabel "t, s"
set xlabel "amp, V"
set terminal x11

plot [0:0.001] \
 "lockin1.txt" using 1:($2*100) with lines,\
 "lockin1.txt" using 1:($3) with lines,\

pause -1