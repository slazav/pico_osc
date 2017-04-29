#!/usr/bin/gnuplot

set terminal x11
splot "ex1ch_sfft.txt" using 1:2:(sqrt($3**2+$4**2)) with lines
pause -1

splot "ex2ch_sfft.txt" using 1:2:(sqrt($3**2+$4**2)) with lines
pause -1

splot "test1_sfft.txt" using 1:2:(sqrt($3**2+$4**2)) with lines
pause -1

splot "test2_sfft.txt" using 1:2:(sqrt($3**2+$4**2)) with lines
pause -1
