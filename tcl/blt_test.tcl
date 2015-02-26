#!/rota/software/bin/wish

# lappend auto_path /rota/programs/tcl_packages
package require BLT
package require http

blt::graph .p -highlightthickness 0
pack .p -fill both -expand yes -side top

checkbutton .b -text Crosshairs -variable v
pack .b -side top

checkbutton .b2 -text Readout -variable v2 
pack .b2 -side top

entry .m -textvariable mes -relief flat -state readonly \
    -width 50 -font [.b2 cget -font] -highlightthickness 0
pack .m -side top
proc show_message {s args} {set ::mes [string map {\n " "} $s]}

.p marker create text -name xmark -text "Readout enabled" \
    -coords {4.7 0} -bg green
.p marker create text -name ymark -text "Readout\ndisabled" \
    -coords {7 0.5} -bg red

proc show_rect {graph x1 x2 y1 y2} {
    show_message "$x1 -- $x2"
}

bind .p <Enter> {focus %W}



set token [http::geturl "localhost:8081/set_chan?channel=A&coupling=DC&range=0.4"]
set token [http::geturl "localhost:8081/set_chan?channel=B&coupling=DC&range=0.4"]
set token [http::geturl "localhost:8081/set_gen?volt=0.35&f1=30&f2=30&cycles=5&trig_src=soft"]
set token [http::geturl "localhost:8081/set_trig?src=A&dir=rising&enable=1&thr=100"]
set token [http::geturl "localhost:8081/rec_block?rate=3000&pretrig=0.2&time=0.2&triggen"]
upvar #0 $token arr
foreach {tag value} $arr(meta) { set meta.$tag $value }
puts "status: ${meta.Status}"
puts "length: ${meta.Content-Length}"
puts $arr(meta)
#puts "trig_time: ${meta.TrigTime}"

set len [expr {${meta.Content-Length}/4}]
binary scan $arr(body) s${len}s${len} a b

blt::vector create xd yd y2d
xd seq 1 $len 1

.p element create d -xdata xd -ydata $a -symbol {} -color blue
.p element create d2 -xdata xd -ydata $b -symbol {} -color red

# xblt::readout::set_axis_format .p x "%.3f rad"
# xblt::readout::set_element_format .p d2 "%.4f cm"
