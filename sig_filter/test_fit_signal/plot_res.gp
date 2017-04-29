#!/usr/bin/gnuplot

set title "Frequency, relative error"
plot  \
 "res.txt" using 0:($1/32674-1) pt 7 title "fre"

pause -1

set title "Relaxartion time, relative error"
plot  \
 "res.txt" using 0:(1/$2/0.925-1) pt 7 title "tau"
pause -1


set title "Amplitude, relative error"
plot  \
 "res.txt" using 0:($3/0.582-1) pt 7 title "amp"

pause -1

set title "Phase"
plot  \
 "res.txt" using 0:($4) pt 7 title "amp", 0.1234

pause -1
