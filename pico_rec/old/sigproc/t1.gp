#!/usr/bin/gnuplot

#lor1(x, a,b,f,t) = (b/t - a*2*pi*(f-x))/((2*pi*(f-x))**2 + 1/t**2)
#lor2(x, a,b,f,t) = -(a/t + b*2*pi*(f-x))/((2*pi*(f-x))**2 + 1/t**2)

#plot[32260:32320]\
#  "t1.txt" using 1:2 with points pt 6,\
#  "t1.txt" using 1:3 with points pt 6,\
#  lor1(x, -8609.24, -52717.6, 32287.9, 0.189091),\
#  lor2(x, -8609.24, -52717.6, 32287.9, 0.189091)

plot\
  "t1.txt" using 1:2 with lines
pause -1

plot\
  "t1.txt" using 1:3 with lines
pause -1