#!/rota/software/bin/wish

# a simple tcl script for controlling the interface

set prog "pico.tcl"

# configure channels:
send $prog chA configure -enable 1 -coupling DC -range 2 -offset 0.1
send $prog chB configure -enable 0

# configure trigger:
send $prog trg configure\
   -enable 1 -src A -dir FALLING -thr 0.1 -del 10 -autotrig 10

#configure generator:
send $prog gen configure\
  -volt 1 -offset 0 -wave SINE  -freq 100  -cycles 0\
  -trig_src NONE -trig_dir RISING -trig_thr 0

#configure recording:
send $prog rec configure\
  -points 2048 -time 0.02 -pretrig 20 -avrg 5 -fft 0

# print some parameter (points from the record panel):
puts [send $prog rec cget -points]

# record the signal
send $prog run

after idle exit
