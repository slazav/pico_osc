#!/usr/bin/gnuplot

plot "ex1ch_fft.txt" using 1:(sqrt($2**2+$3**2)) with lines
pause -1

plot "ex2ch_fft.txt" using 1:(sqrt($2**2+$3**2)) with lines
pause -1

plot "test1_fft.txt" using 1:(sqrt($2**2+$3**2)) with lines
pause -1

plot "test2_fft.txt" using 1:(sqrt($2**2+$3**2)) with lines
pause -1
