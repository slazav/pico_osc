#!/usr/bin/gnuplot

set title "fork response"
set xlabel "t, s"
set xlabel "amp, V"

plot [-0.0001:0.0001] [] \
 "fork2.txt" using 1:($2*200) with linespoints pt 7,\
 "fork2.txt" using 1:3 with linespoints pt 7,\

pause -1