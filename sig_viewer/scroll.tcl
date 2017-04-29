# connects a scrollbar with a blt graph

namespace eval xblt::scroll {
  variable blt_plot
  variable scrollbar
}

proc xblt::scroll {graph sbar args} {
  set xblt::scroll::blt_plot  $graph
  set xblt::scroll::scrollbar $sbar
  $xblt::scroll::scrollbar configure -orient horizontal -command "xblt::scroll::cmd"
  $xblt::scroll::blt_plot  axis configure x -scrollcommand "xblt::scroll::sset"

  bind . <Key-End>   "xblt::scroll::cmd moveto 1"
  bind . <Key-Home>  "xblt::scroll::cmd moveto 0"
  bind . <Key-Left>  "xblt::scroll::cmd scroll -1 units"
  bind . <Key-Right> "xblt::scroll::cmd scroll 1 units"
  bind . <Key-Prior> "xblt::scroll::cmd scroll -1 pages"
  bind . <Key-Next>  "xblt::scroll::cmd scroll 1 pages"
  bind . <ButtonPress-4> "xblt::scroll::cmd scroll -1 units"
  bind . <ButtonPress-5> "xblt::scroll::cmd scroll 1 units"
}

proc xblt::scroll::sset {x1 x2} {
  $xblt::scroll::scrollbar set $x1 $x2
}

proc xblt::scroll::cmd {args} {

  ## blt plot limits
  set xlim [$xblt::scroll::blt_plot axis limits x]
  set xmin [lindex $xlim 0]
  set xmax [lindex $xlim 1]

  ## scrollbar limits
  set slim [$xblt::scroll::scrollbar get]
  set smin [lindex $slim 0]
  set smax [lindex $slim 1]

  set s $smin
  set sbw [expr $smax-$smin]

  switch -exact [lindex $args 0] {
    moveto {
      set s [lindex $args 1]
    }
    scroll {
      set n [lindex $args 1]
      if {[string equal [lindex $args 2] units]} {
        set s [expr $s+$n*$sbw/20]
      } else {
        set s [expr $s+$n*$sbw]
      }
    }
  }
  if {$s > 1-$sbw} {set s [expr 1-$sbw]}
  if {$s < 0} {set s $smin}
  set k [expr {($xmax-$xmin)/($smax-$smin)}]
  set xmin [expr {$xmin + ($s-$smin)*$k}]
  set xmax [expr {$xmax + ($s-$smin)*$k}]
  $xblt::scroll::blt_plot axis configure x -min $xmin -max $xmax
}

