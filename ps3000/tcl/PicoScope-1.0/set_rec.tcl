# set record widget -- passive widget, no actions

package require Itcl 3
namespace eval pscope {}

itcl::class pscope::set_rec {

  public variable points 1024;  # number of points
  public variable time   0.01;    # record time, s
  public variable pretrig 10;   # pretrig, %
  public variable triggen 1;    # trigger the generator
  public variable avrg    1;    # number of averages
  public variable fft     0;    # number of averages
  public variable bg            "#E0F0E0"

  constructor { root } {

    labelframe $root -text "Record"

    label $root.points_l  -text "Points:" -anchor w
    entry $root.points -textvariable [itcl::scope points] -width 8
    label $root.time_l  -text "Time, s:" -anchor w
    entry $root.time -textvariable [itcl::scope time] -width 8
    label $root.pretrig_l  -text "Pretrig, %:" -anchor w
    entry $root.pretrig -textvariable [itcl::scope pretrig] -width 8
    label $root.avrg_l  -text "average:" -anchor w
    entry $root.avrg -textvariable [itcl::scope avrg] -width 8
    checkbutton $root.triggen -variable [itcl::scope triggen]\
       -text "Trigger the generator" -anchor w
    checkbutton $root.fft -variable [itcl::scope fft]\
       -text "FFT" -anchor w

    grid $root.points_l $root.points -sticky we
    grid $root.time_l $root.time -sticky we
    grid $root.pretrig_l $root.pretrig -sticky we
    grid $root.avrg_l $root.avrg -sticky we
    grid $root.triggen -sticky we -columnspan 2
    grid $root.fft -sticky we -columnspan 2

    foreach ch [winfo children $root] { $ch configure -bg $bg }
    $root configure -bg $bg
  }

  method get_cmd {} {
    return [list rec_block\
      points=$points\
      time=$time\
      pretrig=$pretrig\
      triggen=$triggen\
      avrg=$avrg\
      fft=$fft\
    ]
  }

}
