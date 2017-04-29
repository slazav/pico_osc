# shift and rescale plots
# 2nd button on plot: shift and rescale

namespace eval xblt::elemop {
  variable data
}

proc xblt::elemop {graph args} {
  eval xblt::elemop::add $graph $args
}

proc xblt::elemop::add {graph args} {
  bind xblt::elemop::data <ButtonPress-2>   "xblt::elemop::start $graph %x %y"
  bind xblt::elemop::data <B2-Motion>       "xblt::elemop::do    $graph %x %y"
  bind xblt::elemop::data <ButtonRelease-2> "xblt::elemop::end   $graph %x %y"
  xblt::bindtag::add $graph xblt::elemop::data
  set xblt::elemop::data(started) 0
}

######################################################################
proc xblt::elemop::start {graph xp yp} {
  $graph element closest $xp $yp e -interpolate yes
  set xblt::elemop::data(es) $e(name)
  if {$xblt::elemop::data(es) == ""} return
  xblt::crosshairs::show $graph 0
  set xblt::elemop::data(xpi) $xp
  set xblt::elemop::data(yi) [$graph axis invtransform $xblt::elemop::data(es) $yp]
  set xblt::elemop::data(sc) 1.0
  set xblt::elemop::data(started) 1
  return -code break
}

proc xblt::elemop::do {graph xp yp} {
  if {! $xblt::elemop::data(started)} return
  set v $xblt::elemop::data(es)
  set yi $xblt::elemop::data(yi)
  set y [$graph axis invtransform $v $yp]
  set y1 [lindex [$graph axis limits $v] 0]
  set y2 [lindex [$graph axis limits $v] 1]
  set sc [expr {exp(($xp - $xblt::elemop::data(xpi)) / 100.0)}]

  if {[$graph axis cget $v -logscale]} {
    set newmin [expr {exp(log($yi) - (log($y) - log($y1))*$xblt::elemop::data(sc)/$sc)}]
    set newmax [expr {exp(log($yi) + (log($y2) - log($y))*$xblt::elemop::data(sc)/$sc)}]
  } else {
    set newmin [expr {$yi - ($y - $y1)*$xblt::elemop::data(sc)/$sc}]
    set newmax [expr {$yi + ($y2 - $y)*$xblt::elemop::data(sc)/$sc}]
  }
  $graph axis configure $v -min $newmin -max $newmax
  set xblt::elemop::data(sc) $sc
  return -code break
}

proc xblt::elemop::end {graph xp yp} {
  if {! $xblt::elemop::data(started)} return
  set xblt::elemop::data(started) 0
  xblt::crosshairs::show $graph 1
  return -code break
}

