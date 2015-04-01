# plot widget

package require Itcl 3
package require xBlt 3

namespace eval pscope {}

itcl::class pscope::plot_panel {
  inherit pscope::interface

  variable pl;           # plot widget
  variable rate 10000;   # rate, Hz
  variable tpre 0;       # pretrigger time
  variable tpst 0.01;    # record time after the trigger event
  variable triggen 1;    # trigger the generator
  variable avrg 1;       # number of averages

  constructor { root } {

    # the plot widget itself
    labelframe $root -text "Plot"
    set pl $root.plot
    blt::graph $pl
    pack $pl -fill both -expand yes -side top

    ## a frame with setting below the plot
    frame $root.g
    pack $root.g

    label $root.g.rate_l  -text "Rate, Hz:" -anchor w
    entry $root.g.rate -textvariable [itcl::scope rate] -width 8
    label $root.g.tpre_l  -text "pretrig, s:" -anchor w
    entry $root.g.tpre -textvariable [itcl::scope tpre] -width 8
    label $root.g.tpst_l  -text "time, s:" -anchor w
    entry $root.g.tpst -textvariable [itcl::scope tpst] -width 8
    label $root.g.avrg_l  -text "averages:" -anchor w
    entry $root.g.avrg -textvariable [itcl::scope avrg] -width 8

    grid $root.g.rate_l $root.g.rate\
         $root.g.tpre_l $root.g.tpre\
         $root.g.tpst_l $root.g.tpst\
         $root.g.avrg_l $root.g.avrg

    ## a place for messages
#    entry $root.mes -textvariable mes -relief flat -state readonly \
#	-width 50  -highlightthickness 0
#    proc show_message {s args} {set ::mes [string map {\n " "} $s]}
#    pack $root.mes -side top

    ## plot menues
    xblt::plotmenu   $pl -showbutton 1 -buttonlabel "Menu" -menuoncanvas 0
    xblt::crosshairs $pl -usemenu 1 -variable v
#    xblt::readout    $pl -usemenu 1 -variable v2 -active 1 -interpolate yes \
#                        -onmarkers x* -eventcommand {1 {[itcl::scope show_message]}}
    xblt::hielems    $pl -usemenu 1

    xblt::measure    $pl -usemenu 1 -event <Key-equal> -quickevent <Alt-1> \
                        -command show_message -commandlabel "Save reading"

    xblt::zoomstack  $pl -usemenu 1 -scrollbutton 2 -axes {x y}
                     $pl axis configure y2 -hide 0

    $pl legend configure -activebackground white
    set m [xblt::legmenu $pl -showseparator 1]
    $m add command -label Delete -command {
        $pl element delete [xblt::legmenu::curelem $pl]
    }

    xblt::rubberrect::add $pl -type x -modifier Shift \
        -configure {-outline blue} \
        -invtransformx x -command show_rect ;# -cancelbutton ""
    proc show_rect {graph x1 x2 y1 y2} { show_message "$x1 -- $x2" }

    bind $pl <Enter> {focus %W}
  }

  method read_data {} {
    set token [run_cmd rec_block rate=$rate\
        pretrig=$tpre time=$tpst triggen=$triggen avrg=$avrg]
    upvar #0 $token arr

    foreach {tag value} $arr(meta) { set meta.$tag $value }
    puts $arr(meta)
    puts ${meta.TrigSamp}

    set len [expr {${meta.Content-Length}/4}]
    binary scan $arr(body) s${len}s${len} a b

    blt::vector create xx
    xx seq -${meta.TrigSamp} $len 1
    if { [$pl element exists A] } { $pl element delete A}
    if { [$pl element exists B] } { $pl element delete B}
    if { [$pl element exists Trig] } { $pl element delete Trig}
    if { [$pl element exists Over] } { $pl element delete Over}
    $pl element create A -xdata xx -ydata $a -symbol {} -color blue
    $pl element create B -xdata xx -ydata $b -symbol {} -color red

    $pl element create Trig -xdata {0 0} -ydata {-32512 32512} -symbol {} -color black
    if { ${meta.Overload} } {
      $pl element create Over -xdata {$xx(1) $xx($len) $xx($len) $xx(1)}\
                              -ydata {32512 32512 -32512 -32512 }\
                              -symbol {} -color magenta
    }
    set [itcl::scope rate] [expr "1/${meta.DT}"]

  }

}


# blt::vector create xd yd y2d y3d
# xd seq 0 8 0.05
# yd expr sin(xd)
# y2d expr cos(xd)
# y3d expr sin(xd)^2
# 
# $pl element create d  -xdata xd -ydata yd -symbol {} -color blue
# $pl element create d2 -xdata xd -ydata y2d -symbol {} -color red
# $pl element create d3 -xdata xd -ydata y3d -symbol {} -color green
# 
# xblt::readout::set_axis_format $pl x "%.3f rad"
# xblt::readout::set_element_format $pl d "%.4f cm"
# xblt::readout::set_element_format $pl d2 "%.4f mm"

