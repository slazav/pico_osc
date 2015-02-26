#!/rota/software/bin/wish

package require Tclx 8.0; # signal
package require BLT 2.4;  # frames

set setup:dev  "";        # default device serial number (empty for autodetect)
set setup:prog "../pico"; # location of the pico command line interface
set setup:pico "";        # interface pipe descriptor
set url "localhost:8081";

############################################################
#### exit handler
signal trap {SIGTERM SIGINT} onexit
wm protocol . WM_DELETE_WINDOW onexit
proc onexit {} {
  setup:disconnect
  puts "Exiting..."
  exit 0
}

############################################################
#### setup commands
proc setup:connect {} {
  global setup:dev setup:prog setup:pico
  setup:disconnect
  if { ![file exists ${setup:prog}] } { return }
  set setup:pico [ open "| ${setup:prog} -d '${setup:dev}' int" w+ ]
#  fconfigure ${setup:pico} -blocking 0
  puts "Running pico interface, pid: [ pid ${setup:pico} ]"
}
proc setup:disconnect {} {
  global setup:pico
  if { ${setup:pico} eq "" } {return}
  if {[catch {tell ${setup:pico}}] != 0} {return}
  close ${setup:pico};
  puts "Closing pico interface..."
  set ${setup:pico} "";
}
proc setup:search {} {
  global setup:pico
  after 500
  puts ${setup:pico} "info\n"
  after 500
#  while {[gets ${setup:pico} line] >= 0} {
#puts ">> $line"
#    .tabs.setup.search insert end "$line\n" }


#  while { ![fblocked ${setup:pico}] } {
#    gets ${setup:pico} line
#    .tabs.setup.search insert end "$line\n" }
  .tabs.setup.search see end
}

############################################################
#### useful function for the interface construction:
# make a top-level frame
proc mkframe {group} {
  frame .tabs.$group
  .tabs insert end $group -window .tabs.$group\
     -anchor n -text $group -fill x
}
# make simple entry and pack it into the frame
proc mkentry {fr name title} {
  set var $fr:$name
  global $var
  label .tabs.$fr.${name}_l  -text "$title" -anchor w
  entry .tabs.$fr.$name -textvariable $var
  grid  .tabs.$fr.${name}_l .tabs.$fr.$name
}

# make simple checkbutton
proc mkcheckb {fr name title} {
  set var $fr:$name
  global $var
  checkbutton .tabs.$fr.${name} -variable $var -text "$title" -anchor w
  grid  x .tabs.$fr.${name}
}
# make and pack optionMenu widget
proc mkopts {fr name title items} {
  set var $fr:$name
  global $var
  label .tabs.$fr.${name}_l  -text "$title" -anchor w
  eval tk_optionMenu .tabs.$fr.$name $var $items
  grid  .tabs.$fr.${name}_l .tabs.$fr.$name -sticky we
}

# make simple button and pack it into the frame
proc mkbtn {fr name title} {
  button .tabs.$fr.$name -text "$title" -command $fr:$name
  grid .tabs.$fr.$name - -pady 10 -padx 10 -sticky e
}
# make search panel
proc mksearch {fr name title} {
  label  .tabs.$fr.${name}_l -text "$title"
  text   .tabs.$fr.${name} -height 4 -width 30 -wrap word
  button .tabs.$fr.${name}_b -text "Search" -command $fr:$name
  grid   .tabs.$fr.${name}_l x
  grid   .tabs.$fr.${name} - -sticky we
  grid   .tabs.$fr.${name}_b - -pady 10 -padx 10 -sticky e
}


############################################################
#### make frames
blt::tabset .tabs -side left
mkframe channels
mkframe generator
mkframe osc
mkframe pulse
mkframe setup
pack .tabs -fill both -expand yes

############################################################
#### channels frame
pack [labelframe .tabs.channels.a -text "A"]
pack [labelframe .tabs.channels.b -text "B"]
mkcheckb channels.a enable "Enable"
mkopts   channels.a coupling "Coupling:" {AC DC}
mkopts   channels.a range "Rangs, Vpp:" {100 40 20 10 4 2 1 0.4 0.2 0.1 0.04 0.02 0.01}
mkentry  channels.a offset "Offset, V:"

mkcheckb channels.b enable "Enable"
mkopts   channels.b coupling "Coupling:" {AC DC}
mkopts   channels.b range "Rangs, Vpp:" {100 40 20 10 4 2 1 0.4 0.2 0.1 0.04 0.02 0.01}
mkentry  channels.b offset "Offset, V:"
#mkbtn    channels apply "Apply"

proc channels:apply {} {
  global urlpref
  global channels.a:coupling channels.a:range channels.a:offset channels.a:enable
  global channels.b:coupling channels.b:range channels.b:offset channels.b:enable
  set token [http::geturl "$url/set_chan?channel=A&coupling=channels.a:coupling&enable&range=0.4"]

  if { ![file exists ${setup:prog}] } { return }
  set setup:pico [ open "| ${setup:prog} -d '${setup:dev}' int" w+ ]
#  fconfigure ${setup:pico} -blocking 0
  puts "Running pico interface, pid: [ pid ${setup:pico} ]"
}


############################################################
#### setup frame
mkentry  setup dev  "Device serial number"
mkentry  setup prog "Command line interface"
mkbtn    setup connect "Reconnect"
mksearch setup search "Find devices"


############################################################
setup:connect
set status idle
