#!/rota/software/bin/wish

lappend auto_path .
package require xBlt 3
package require PicoScope 1

frame .p0
frame .p1
frame .p2

pscope::plot_panel plot .p0.plot
pscope::set_chan chA .p1.chA A
pscope::set_chan chB .p1.chB B
pscope::set_rec  rec .p1.rec
pscope::set_trig trg .p2.trg
pscope::set_gen  gen .p2.gen

entry  .p0.stat -textvariable status
button .p1.run  -text "Run" -command {run}

pack .p0.plot .p0.stat               -fill both -expand 1
pack .p1.chA .p1.chB .p1.rec .p1.run -fill both -expand 1
pack .p2.trg .p2.gen                 -fill both -expand 1

grid .p0 .p1 .p2 -sticky ns

blt::vector create Xdat
blt::vector create Adat
blt::vector create Bdat


# process HTTP request and process results
proc run_cmd {w} {
  global status
  # build HTTP adress for the command
  set addr     localhost:8081
  set cmdlist [$w get_cmd]
  set cmd  [lindex $cmdlist 0]
  set args [lrange $cmdlist 1 end]
  set args [join $args &]

  # run the command
  if {0} { puts "$addr/$cmd?$args" }
  set token [http::geturl "$addr/$cmd?$args"]

  # set status
  upvar #0 $token arr
  foreach {tag val} $arr(meta) {
    if { $tag eq "Status" && $val ne "OK" } {
      set status "$cmd: $val"
    }
  }
  return $token
}

## run button proc
proc run {} {
  global chA chB gen trg rec
  global status plot
  set pl  [plot cget -w]
  set fft [rec cget -fft]

  # disable Run button
  .p1.run configure -state disabled
  set status "Recording the signal"

  # apply settings from all widgets:
  foreach w {chA chB trg gen rec} { run_cmd $w }

  # we need response from the rec widget
  set token [ run_cmd rec ]
  upvar #1 $token arr
  #puts $arr(meta)

  # set variables from the HTTP metadata
  set len 0
  set trig_samp 0
  set trig_time 0
  set overload  0
  set dt        0
  foreach {tag val} $arr(meta) {
    if { $tag eq "Content-Length" } { set len       $val }
    if { $tag eq "TrigSamp" }       { set trig_samp $val }
    if { $tag eq "TrigTime" }       { set trig_time $val }
    if { $tag eq "Overload" }       { set overload  $val }
    if { $tag eq "DT" }             { set dt        $val }
  }

  # Split the HTTP body into A and B arrays.
  # In fft mode we have 8-byte double values,
  # otherwise, 2-byte integers.
  if { $fft } {
    set len [expr {$len/16}];
    binary scan $arr(body) d${len}d${len} a b
  } else {
    set len [expr {$len/4}];
    binary scan $arr(body) s${len}s${len} a b
  }

  Adat set $a
  Bdat set $b
  Xdat seq 0 $len 1

  set max 32512.0
  set maxA [expr [ chA cget -range ] / 2.0 ]
  set maxB [expr [ chB cget -range ] / 2.0 ]
  Xdat expr (Xdat-$trig_samp)*$dt+$trig_time
  Adat expr Adat/$max
  Bdat expr Bdat/$max
#  Adat expr Adat*[expr $maxA/$max]
#  Bdat expr Bdat*[expr $maxB/$max]

  # refresh the plot
  if { ! [$pl element exists A] } {
    $pl element create A -xdata Xdat -ydata Adat -symbol {} -color red }
  if { ! [$pl element exists B] } {
    $pl element create B -xdata Xdat -ydata Bdat -symbol {} -color blue }


  if { [$pl element exists Trig] } { $pl element delete Trig}
  if { [$pl element exists Over] } { $pl element delete Over}
  $pl element create Trig -xdata {0 0} -ydata {-1 1} -symbol {} -color black
  if { $overload } {
    $pl element create Over -xdata {$Xdat(1) $Xdat($len) $Xdat($len) $Xdat(1)}\
                            -ydata {1 1 -1 -1 }\
                            -symbol {} -color magenta
    set status "Overload"
  }
  rec configure -time [expr "$len*$dt"]

  # enable Run button
  .p1.run configure -state normal
}

