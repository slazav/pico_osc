## pico_osc -- record signals using Picoscope

Source code: `https://github.com/slazav/graphene`

E-mail: `Vladislav Zavjalov <slazav@altlinux.org>`

Now only PS4224 model is supported.

###Usage: pico_osc [options] < commands

###Options:
`
 -d <dev>  -- device ID (autodetect by default)
 -l        -- list all connected devices and exit
 -h        -- write this help message and exit
`

The program implements a simple pipe protocol (see somewhere in my
tcl-device package): When it is started and an oscilloscope device is opened
a prompt message is printed to stdout started with "#SPP001" and followed
by "#OK" line. In case of an error "#Error: <...>" line is printed and
program exits.

If the oscilloscope have been opened sucsessfully, the program reads
commands from stdin and sends ansers to stdout folowed by "#OK" or
"#Error: <...>" lines until the user  close the connection.

There is one non-trivial exception in the protocol: the "block" command
returns two "#OK" lines: one then the device is ready for the signal
recording, and second then the recording is done and the signal is
written to a file. User should care about reading both of them.

###Commands (see pico_osc -h):
`
help -- show command list

ranges <ch> -- show possible ranges
   ch  -- select channel: A,B

chan_set <ch> <en> <cpl> <rng> -- set channel parameters
   ch  -- select channels: A, B, AB, etc.
   en  -- enable channel: 1,0
   cpl -- coupling: AC, DC
   rng -- input range, volts (see ranges command)
Answer: OK or error message.

chan_get <ch>  -- get channel parameters
   ch  -- select channel: A, B, AB etc.
Returns a line with four words (<ch> <en> <cpl> <rng>) for each channel.
If the channel is not set returns channel name with 'undef' word.

trig_set <src> <lvl> <dir> <del> -- set channel parameters
   src -- source: A,B,EXT,NONE
   lvl -- threshold level, ratio of full range, -1..1
   dir -- direction: RISING,FALLING,ABOVE,BELOW,RISING_OR_FALLING
   del -- delay, samples
Answer: OK or error message.

trig_get  -- get trigger parameters
  Returns a line with four words: <src> <lvl> <dir> <del>,
  If the trigger is not set returns 'undef' word.

block <ch> <npre> <npost> <dt> <file> -- record signal (block mode)
   ch    -- channels to record: A,B,AB,BA, etc."
   npre  -- number of pretrigger samples
   npost -- number of posttrigger samples
   dt    -- time step, seconds
   file  -- output file (do not write anything if file is -)
   Answer: OK when trigger can be run. To wait until the signal is
   recorded and written to a file use wait command after the block command.
   If filename is "-" then file is not written (useful in averaging).

wait -- wait until the oscilloscope is ready and return status
   of last block command.

avr_start -- start averaging
   If averging is on, sum of all recorded signals is collected in a separate buffer.
   Number of points of all signals should be the same.
   Use avr_start to reset buffers and start averaging.

avr_stop -- stop averaging, reset averaging buffers

avr_save <fname> -- save averaged signal (should be done before avr_start or avr_stop)

filter <file> <args> -- run sig_filter program

*idn? -- write id string: "pico_osc <VERSION>"

get_time -- print current time (unix seconds with ms precision)
`
