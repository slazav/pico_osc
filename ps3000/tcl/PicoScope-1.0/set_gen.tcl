# set_gen widget -- passive widget, no actions
#   sweeps are not supperted yet

package require Itcl 3
namespace eval pscope {}

itcl::class pscope::set_gen {
  public variable volt     1
  public variable offset   0
  public variable wave     SINE
  public variable freq     1000
  public variable cycles   0

  public variable trig_src NONE
  public variable trig_dir RISING
  public variable trig_thr 0
  public variable bg       "#F0E0F0"

  constructor { root } {

    labelframe $root -text "Generator"

    label $root.volt_l  -text "Amplitude, Vpp:" -anchor w
    entry $root.volt -textvariable [itcl::scope volt] -width 8

    label $root.offset_l  -text "Offset, V:" -anchor w
    entry $root.offset -textvariable [itcl::scope offset] -width 8

    label $root.wave_l  -text "Waveform:" -anchor w
    eval tk_optionMenu $root.wave {[itcl::scope wave]}\
       {SINE SQUARE TRIANGLE}

    label $root.freq_l  -text "Frequncy, Hz:" -anchor w
    entry $root.freq -textvariable [itcl::scope freq] -width 8

    label $root.cycles_l  -text "Cycles:" -anchor w
    entry $root.cycles -textvariable [itcl::scope cycles] -width 8

    label $root.trig_src_l  -text "Trigger source:" -anchor w
    eval tk_optionMenu $root.trig_src {[itcl::scope trig_src]}\
       {NONE SCOPE EXT SOFT}

    label $root.trig_dir_l  -text "Trigger direction:" -anchor w
    eval tk_optionMenu $root.trig_dir {[itcl::scope trig_dir]}\
       {RISING FALLING HI LOW}

    label $root.trig_thr_l  -text "Trigger threshold:" -anchor w
    entry $root.trig_thr -textvariable [itcl::scope trig_thr] -width 8

    grid $root.freq_l     $root.freq -sticky we
    grid $root.volt_l     $root.volt -sticky we
    grid $root.offset_l   $root.offset -sticky we
    grid $root.wave_l     $root.wave -sticky we
    grid $root.cycles_l   $root.cycles -sticky we
    grid $root.trig_src_l $root.trig_src -sticky we
    grid $root.trig_dir_l $root.trig_dir -sticky we
    grid $root.trig_thr_l $root.trig_thr -sticky we

    foreach ch [winfo children $root] { $ch configure -bg $bg }
    $root configure -bg $bg
  }

  method get_cmd {} {
    return [list set_gen\
      volt=$volt\
      offset=[expr $offset*1e6]\
      wave=$wave\
      f1=$freq\
      f2=$freq\
      cycles=$cycles\
      sweeps=0\
      trig_src=$trig_src\
      trig_dir=$trig_dir\
      trig_thr=$trig_thr\
    ]
  }

}


