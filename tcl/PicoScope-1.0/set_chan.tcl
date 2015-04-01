# set_chan widget

package require Itcl 3
namespace eval pscope {}

itcl::class pscope::set_chan {
  inherit pscope::interface
  public variable chan     A
  public variable enable   1
  public variable coupling DC
  public variable range    40
  public variable offset   0

  constructor { root ch} {

    set chan $ch
    labelframe $root -text "Channel $ch"

    checkbutton $root.enable -variable [itcl::scope enable] -text "Enable" -anchor w

    label $root.coupling_l  -text "Coupling:" -anchor w
    eval tk_optionMenu $root.coupling {[itcl::scope coupling]}\
       {AC DC}

    label $root.range_l  -text "Range:" -anchor w
    eval tk_optionMenu $root.range {[itcl::scope range]}\
       {40 20 10 4 2 1 0.4 0.2 0.1}

    label $root.offset_l  -text "Offset, V:" -anchor w
    entry $root.offset -textvariable [itcl::scope offset] -width 8

    grid $root.enable
    grid $root.coupling_l $root.coupling -sticky we
    grid $root.range_l    $root.range  -sticky we
    grid $root.offset_l   $root.offset -sticky we
  }

  method apply {} {
    run_cmd set_chan\
      channel=$chan\
      enable=$enable\
      coupling=$coupling\
      range=$range\
      offset=$offset
  }

}


