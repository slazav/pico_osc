#!/usr/bin/wish

# a simple tcl script for controlling the interface
lappend auto_path .
package require MatFile 1.0

matfile::save test.mat {int16 arr {1 2 3 4 5 6 7 8 9 10}}

after idle exit
