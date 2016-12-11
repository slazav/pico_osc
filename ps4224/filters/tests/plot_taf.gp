#!/usr/bin/gnuplot

plot "ex1ch_taf.txt" using 1:3 with linespoints pt 7
pause -1

plot "ex2ch_taf.txt" using 1:3 with linespoints pt 7
pause -1

plot "test1_taf.txt" using 1:2 with linespoints pt 7,\
  0.582 * exp(-x/0.325)
pause -1

plot "test1_taf.txt" using 1:3 with linespoints pt 7,\
  32674
pause -1

plot "test2_taf.txt" using 1:2 with linespoints pt 7,\
  0.582 * exp(-x/0.325)
pause -1

plot "test2_taf.txt" using 1:3 with linespoints pt 7,\
  32674 + 1000*exp(-x/0.12)

pause -1

#    double dt = 1e-5;
#    double f0 = 32674;
#    double tau = 0.325;
#    double amp = 0.582;
#    double noise  = 0;

#    double ftau = 0.112;
#    double famp = 0;


