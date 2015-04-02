# Tcl package index file, version 1.1
# This file is NOT generated by the "pkg_mkIndex" command

set _name PicoScope
set _version 1.0
set _files {interface set_chan set_trig set_gen set_rec plot_panel}
set _pcmd {}

foreach _f $_files { lappend _pcmd "source [file join $dir $_f.tcl]" }
lappend _pcmd "package provide $_name $_version"
package ifneeded $_name $_version [join $_pcmd \n]

