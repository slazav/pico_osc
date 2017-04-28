## pico_rec -- record signals using Picoscope

Source code: `https://github.com/slazav/graphene`

E-mail: `Vladislav Zavjalov <slazav@altlinux.org>`

Now only PS4224 model is supported.

###Usage: pico_rec [options] < commands

###Options:
`
 -d <dev>  -- device ID (autodetect by default)
 -l        -- list all connected devices and exit
 -h        -- write this help message and exit
`

The program implements a simple command line interface: When it is
started and an oscilloscope device is opened a prompt message is printed
to stdout followed by "#OK" line. In case of an error "#Error: <...>"
line is printed and program exits.

If the oscilloscope have been opened sucsessfully, the program reads
commands from stdin and sends ansers to stdout folowed by "#OK" or
"#Error: <...>" lines until the user  close the connection.

There is one non-trivial exceptoin in the protocol: the "block" command
returns two "#OK" lines: one then the device is ready for the signal
recording, and second then the recording is done and the signal is
written to a file. User should care about reading both of them.

###Commands (see pico_rec -h):
`
help -- show command list

ranges <ch> -- show possible ranges
   ch  -- select channel: A,B

chan_set <ch> <en> <cpl> <rng> -- set channel parameters
   ch  -- select channel: A,B
   en  -- enable channel: 1,0
   cpl -- coupling: AC, DC
   rng -- input range, volts (see ranges command)
   Answer: OK or error message.

trig_set <src> <lvl> <dir> <del> -- set channel parameters
   src -- source: A,B,EXT,NONE
   lvl -- threshold level, ratio of full range, -1..1
   dir -- direction: RISING,FALLING,ABOVE,BELOW,RISING_OR_FALLING
   del -- delay, samples
   Answer: OK or error message.

block <ch> <npre> <npost> <dt> <file> -- record signal (block mode)
   ch    -- channels to record: A,B,AB,BA, etc."
   npre  -- number of pretrigger samples
   npost -- number of posttrigger samples
   dt    -- time step, seconds
   file  -- output file
   Answer: Ready when trigger can be run, Done when file is saved.
`
