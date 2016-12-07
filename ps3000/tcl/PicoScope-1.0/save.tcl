# save widget

package require Itcl 3
namespace eval pscope {}

itcl::class pscope::save {
  public variable enable   0
  public variable dir      "."
  public variable fname    "rec.mat"
#  public variable data     RAW
  public variable bg       "#E0E0F0"

  constructor { root } {

    labelframe $root -text "Save"

    checkbutton $root.enable -variable [itcl::scope enable]\
       -text "Enable" -anchor w

    label $root.dir_l  -text "Folder:" -anchor w
    entry $root.dir -textvariable [itcl::scope dir] -width 20

    label $root.fname_l  -text "File name:" -anchor w
    entry $root.fname -textvariable [itcl::scope fname] -width 20

#    label $root.data_l  -text "Data:" -anchor w
#    eval tk_optionMenu $root.data {[itcl::scope data]}\
#       {RAW SCALED}

    grid $root.enable
#    grid $root.data_l  $root.data  -sticky we
    grid $root.dir_l   $root.dir   -sticky we
    grid $root.fname_l $root.fname -sticky we

    foreach ch [winfo children $root] { $ch configure -bg $bg }
    $root configure -bg $bg
  }

}


