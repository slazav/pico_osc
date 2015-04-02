#!/rota/software/bin/wish

# a simple tcl script for controlling the interface
lappend auto_path .
package require MatFile 1.0

set dat {1 2 3 4 5 6}
matfile::save test.mat {int16 arr {1 2 3 4 5 6}}

after idle exit
