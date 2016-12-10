# see: www.mathworks.com/help/pdf_doc/matlab/matfile_format.pdf

namespace eval matfile {}
package require zlib

proc matfile::save { matfile lst} {
  # header:
  # text, <116 chars, 0-ended + 8 zeros + version 0x0100
  #  + endian ("MI" or "IM")
  set head [ binary format a124s1a2 "MATLAB 5.0" 256 IM ]

  set out [open $matfile w]
  puts -nonewline $out $head

  foreach { type name data} $lst {

    # tt -- data type
    # tc -- array type
    # ff -- binay format flag
    # ww -- number of bytes
    if     { [string compare $type   int8] ==0 } { set tt 1;  set tc  8;  set ff c; set ww 1 }\
    elseif { [string compare $type  uint8] ==0 } { set tt 2;  set tc  9;  set ff c; set ww 1 }\
    elseif { [string compare $type  int16] ==0 } { set tt 3;  set tc 10;  set ff s; set ww 2 }\
    elseif { [string compare $type uint16] ==0 } { set tt 4;  set tc 11;  set ff s; set ww 2 }\
    elseif { [string compare $type  int32] ==0 } { set tt 5;  set tc 12;  set ff i; set ww 4 }\
    elseif { [string compare $type uint32] ==0 } { set tt 6;  set tc 13;  set ff i; set ww 4 }\
    elseif { [string compare $type   real] ==0 } { set tt 7;  set tc  7;  set ff f; set ww 4 }\
    elseif { [string compare $type double] ==0 } { set tt 12; set tc  6;  set ff d; set ww 8 }\
    else { puts "Unknown data type: $type"; return }
    set dlen [llength $data]

    # format array flags
    set d_f [ binary format i1i1s1c1c1i1 6 8 0 0 $tc 0 ]

    # format array dimensions
    set d_x [ binary format i1i1i1i1 5 8 1 $dlen ]

    # format array name
    set len [ string length name]
    set d_n [ binary format i1i1a$len 1 $len $name]

    # format array data
    set len [expr $dlen*$ww]; # byte length
    set dat [ binary format i1i1$ff$dlen $tt $len $data]

    # wrap in a matrix
    set dat "$d_f$d_x$d_n$dat"
    set len [string bytelength $dat]
    set d0 [ binary format i1i1 14 $len ]

    # gzip
    if {$dlen > 1} {
      set dat [ zlib compress "$d0$dat" ]
      set len [string bytelength $dat]
      set d0 [ binary format i1i1 15 $len ]
    }
    puts -nonewline $out "$d0$dat"
  }
  close $out

#  set data [zlib inflate [string range $cdata 0 end-8]]
}
