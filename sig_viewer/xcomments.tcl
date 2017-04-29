namespace eval xblt::xcomments {
  variable scom
  variable hidden
  variable dlg_res
  variable data
}

proc xblt::xcomments {graph args} {
  eval xblt::xcomments::add $graph $args
}

proc xblt::xcomments::add {graph args} {
  xblt::parse_options xblt::xcomments::add $args [subst {
    -on_add xblt::xcomments::data($graph,on_add) {}
    -on_del xblt::xcomments::data($graph,on_del) {}
    }]
  set xblt::xcomments::scom(n) 0
  set xblt::xcomments::hidden false
  bind $graph <Control-ButtonPress-1> "xblt::xcomments::create_int $graph %x %y %X %Y"
  xblt::unitaxes::add $graph
}

### create a comment -- interactive version
proc xblt::xcomments::create_int {graph x y X Y} {
  if {$xblt::xcomments::hidden} {return -code break}

  ## ask for a comment text
  set text [xblt::xcomments::ask_text {} $X $Y]
  if {$text == ""} {return -code break}

  ## calculate coordinate
  set xx [$graph axis invtransform x $x]
  xblt::xcomments::create $graph $xx $text
  return -code break
}

### create a comment
proc xblt::xcomments::create {graph xx text} {
  ## create comment data
  set n $xblt::xcomments::scom(n)
  incr xblt::xcomments::scom(n)
  set xblt::xcomments::scom($n,text) $text
  set xblt::xcomments::scom($n,x0)  $xx

  ## create comment markers
  set tm "comm${n}_text"
  set lm "comm${n}_line"

 $graph marker create text -name $tm -hide 1 \
     -background LightGoldenrodYellow \
     -justify left -anchor nw \
     -mapy xblt::unity -text $xblt::xcomments::scom($n,text)\
     -coords [list $xx 1]
  $graph marker create line -name $lm -hide 0 \
     -mapy xblt::unity -outline black\
     -dashes 2 -linewidth 2\
     -coords [list $xx 0 $xx 1]
  $graph marker bind $lm <Enter> [list comment_line_enter $graph $n]
  $graph marker bind $lm <Leave> [list comment_line_leave $graph $n]
  $graph marker bind $lm <Control-ButtonPress-3> [list xblt::xcomments::delete $graph $n]
  set cmd $xblt::xcomments::data($graph,on_add)
  if {$cmd != ""} { uplevel \#0 [eval $cmd $n $xx $text] }
}
proc comment_line_enter {graph n} {
  set tm "comm${n}_text"
  set lm "comm${n}_line"
  $graph marker before $lm
  $graph marker before $tm
#  $graph marker configure $lm -outline red # this causes unexpected Leave event
  $graph marker configure $tm -hide 0
  return -code break
}
proc comment_line_leave {graph n} {
  set tm "comm${n}_text"
  set lm "comm${n}_line"
  $graph marker configure $lm -outline black
  $graph marker configure $tm -hide 1
  return -code break
}


### delete a comment
proc xblt::xcomments::delete {graph n} {
  if {$n == ""} return

  set tm "comm${n}_text"
  set lm "comm${n}_line"

  $graph marker configure $tm -background red -foreground white -hide 0
  $graph marker configure $lm -outline red
  if {[tk_messageBox -type yesno -message "Delete comment?"] == "yes"} {
    set xx   $xblt::xcomments::scom($n,x0)
    set text $xblt::xcomments::scom($n,text)
    set cmd $xblt::xcomments::data($graph,on_del)
    if {$cmd != ""} { uplevel \#0 [eval $cmd $n $xx $text] }

    foreach m [array names xblt::xcomments::scom $n,*] {
      unset xblt::xcomments::scom($m)
    }
    $graph marker delete $tm
    $graph marker delete $lm
  } else {
    $graph marker configure $tm -background LightGoldenrodYellow -foreground black -hide 1
    $graph marker configure $lm -outline black
  }
  return -code break
}

### show a dialog window, ask for a comment text
proc xblt::xcomments::ask_text {old_text X Y} {
  set xblt::xcomments::dlg_res {}
  set w .cominput
  if {![winfo exists $w]} {
    toplevel $w

    text $w.t -background white -width 40 -height 6 -font {helvetica 12}
    bind $w.t <Alt-Key-Return> {set xblt::xcomments::dlg_res 1 ; break}
    bind $w.t <Key-Escape> {set xblt::xcomments::dlg_res 0 ; break}

    button $w.ok -text OK -width 7 -command {set xblt::xcomments::dlg_res 1}
    button $w.can -text Cancel -width 7 -command {set xblt::xcomments::dlg_res 0}

    grid $w.t -columnspan 2 -padx 2 -pady 2
    grid $w.ok $w.can -sticky e -padx 5 -pady 2

    wm transient $w .
    wm protocol $w WM_DELETE_WINDOW {set xblt::xcomments::dlg_res 0}
    wm title $w "Create comment"
  }
  wm geometry $w +$X+$Y
  wm state $w normal
  $w.t delete 1.0 end
  $w.t insert end $old_text
  update idletasks

  focus $w.t
  grab $w
  tkwait variable xblt::xcomments::dlg_res
  grab release $w
  wm state $w withdrawn

  if {$xblt::xcomments::dlg_res == 0} {
    return $old_text
  } else {
    return [string trim [$w.t get 1.0 end]]
  }
}

