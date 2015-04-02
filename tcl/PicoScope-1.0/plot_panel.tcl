# plot widget

package require Itcl 3
package require xBlt 3

namespace eval pscope {}

itcl::class pscope::plot_panel {
  public variable w;           # plot widget

  constructor { root } {
    # the plot widget itself
    labelframe $root -text "Plot"
    set w $root.plot
    blt::graph $w
    pack $w -fill both -expand yes -side top

    ## plot menues
    xblt::plotmenu   $w -showbutton 1 -buttonlabel "Menu" -menuoncanvas 0
    xblt::crosshairs $w -usemenu 1 -variable v
#    xblt::readout    $w -usemenu 1 -variable v2 -active 1 -interpolate yes \
#                        -onmarkers x* -eventcommand {1 {[itcl::scope show_message]}}
    xblt::hielems    $w -usemenu 1

    xblt::measure    $w -usemenu 1 -event <Key-equal> -quickevent <Alt-1> \
                        -command show_message -commandlabel "Save reading"

    xblt::zoomstack  $w -usemenu 1 -scrollbutton 2 -axes {x y}
                     $w axis configure y2 -hide 0

    $w legend configure -activebackground white
    set m [xblt::legmenu $w -showseparator 1]
    $m add command -label Delete -command {
        $w element delete [xblt::legmenu::curelem $w]
    }

    xblt::rubberrect::add $w -type x -modifier Shift \
        -configure {-outline blue} \
        -invtransformx x -command show_rect ;# -cancelbutton ""
    proc show_rect {graph x1 x2 y1 y2} { show_message "$x1 -- $x2" }

    bind $w <Enter> {focus %W}
  }
}
