#!/rota/software/bin/wish

lappend auto_path .
package require xBlt 3
package require PicoScope 1

blt::tabset .tabs -side left
proc mkframe {group} {
  frame .tabs.$group
  .tabs insert end $group -window .tabs.$group\
     -anchor n -text $group -fill x
}
mkframe channels
mkframe trigger
mkframe generator

pscope::set_chan chA .chA A
pscope::set_chan chB .chB B
pscope::set_trig trg .trg
pscope::set_gen  gen .gen
pscope::plot_panel plot .plot

grid .plot -rowspan 3
grid .chA -row 0 -column 1
grid .chB -row 0 -column 2
grid .trg -row 1 -column 1
grid .gen -row 1 -column 2

#grid columnconfigure .plot 0 -weight 1
#grid rowconfigure .plot 0 -weight 1

labelframe .buttons
grid .buttons -row 2 -column 1 -columnspan 2

button .buttons.run -text "Run" -command {run}
pack .buttons.run

proc run {} {
  chA apply
  chB apply
  trg apply
  gen apply
  plot read_data
}


#puts $chA::coupling
#puts $chA::range
#puts $chA::enable

