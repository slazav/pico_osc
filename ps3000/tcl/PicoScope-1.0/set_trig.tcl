# set_trig widget -- passive widget, no actions

package require Itcl 3
namespace eval pscope {}

itcl::class pscope::set_trig {
  public variable src      A
  public variable enable   0
  public variable dir      RISING
  public variable thr      0
  public variable del      0
  public variable autotrig 0
  public variable bg       "#E0F0F0"

  constructor { root } {

    labelframe $root -text "Trigger"

    checkbutton $root.enable -variable [itcl::scope enable] -text "Enable" -anchor w

    label $root.src_l  -text "Source:" -anchor w
    eval tk_optionMenu $root.src {[itcl::scope src]}\
       {A B EXT}

    label $root.dir_l  -text "Direction:" -anchor w
    eval tk_optionMenu $root.dir {[itcl::scope dir]}\
       {RISING FALLING R_or_F ABOVE BELOW}

    label $root.thr_l  -text "Threshold:" -anchor w
    entry $root.thr -textvariable [itcl::scope thr] -width 8

    label $root.del_l  -text "Delay, samples:" -anchor w
    entry $root.del -textvariable [itcl::scope del] -width 8

    label $root.autotrig_l  -text "Autotrigger, s:" -anchor w
    entry $root.autotrig -textvariable [itcl::scope autotrig] -width 8

    grid $root.enable
    grid $root.src_l $root.src -sticky we
    grid $root.dir_l $root.dir -sticky we
    grid $root.thr_l $root.thr -sticky we
    grid $root.del_l $root.del -sticky we
    grid $root.autotrig_l $root.autotrig -sticky we

    foreach ch [winfo children $root] { $ch configure -bg $bg }
    $root configure -bg $bg
  }

  method get_cmd {} {
    return [list set_trig\
      enable=$enable\
      src=$src\
      thr=$thr\
      dir=$dir\
      del=$del\
      autotrig=[expr $autotrig*1000]\
    ]
  }

}


