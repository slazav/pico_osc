#!/usr/bin/gnuplot

set title "1 correlation"
plot [][0:1.5]\
 "dat1.0.nc.txt" using 1:2 with lines,\
 "dat1.0.nc.txt" using 1:3 with lines,\
 "dat1.0.co.txt" using 1:2 with lines

pause -1

set title "1/2 correlation"
plot [][0:1.5]\
 "dat0.5.nc.txt" using 1:2 with lines,\
 "dat0.5.nc.txt" using 1:3 with lines,\
 "dat0.5.co.txt" using 1:2 with lines

pause -1


set title "0 correlation"
plot [][0:1.5]\
 "dat0.0.nc.txt" using 1:2 with lines,\
 "dat0.0.nc.txt" using 1:3 with lines,\
 "dat0.0.co.txt" using 1:2 with lines

pause -1
