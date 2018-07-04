Marco Will:

The PicoProgram is the program which runs the gui (it has one button
which shows if the device is connected and when pressed gives the info
about the device in a new window). It uses the tkinter (basically tcl for
python) package. And then there is the DeviceFunctions file which
contains the functions I call in the program. So far open/close and the
before mentioned info function are implemented. Other functions can be
used utilizing the driver, the principle is shown. I use the
"libpicohrdl.so.2", which is provided by the company
([1]https://www.picotech.com/downloads/linux). Also I was testing it on a
virtual box running Ubuntu and Python 3.6 64bit.
