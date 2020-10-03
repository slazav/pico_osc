# pico_adc program -- SPP interface for Pico ADC24 device.

### Command line options:

* `-l`       -- list all connected devices and exit
* `-d <dev>` -- device ID (autodetect by default)
* `-m <Hz>`  -- set mains frequency, Hz (50 or 60, default 50)
* `-h`       -- write this help message and exit

### SPP commands

* `get_time` -- Get current time (unix seconds with ms precision).

* `*idn?`    -- Get ID string: `pico_adc <version>`.

* `help`     -- Get list of commands.

* `get_info` -- Get device information.

* `ranges`   -- Get available range settings (mV):
                `2500 1250 625 312.5 156.25 78.125 39.0625`.

* `tconvs`   -- Get available conversion time settings (ms):
                `60 100 180 340 660`

* `get_val <chan> <single> <rng> <convt>` -- Measure a single value.
  This command do measurement of a single value on the channel `<chan>`
  (1..16). Boolean parameter `<single>` specifies single/differential mode.
  Only odd channels (1,3,5,...) can be used for differential mode,
  next channel number is used as the second channel.
  Parameter `<rng>` is measurement range, one of values returned by
  `ranges` command. `convt` is conversion time,  one of values returned
  by `tconvs` command. Use longer time for more accurate measurement.

Commands listed below are used for "block read mode", to do measurement
of multiple channels. In this mode one should set channel parameters and
measurement timings in advance, and then do the measurement. Note that
real measurements are still done in series, channel by channel. If device
is used by multible users (e.g. with device2 server) then device locking
is needed during the measurement. It is not recommended to use
block read mode (use `get_val` instead), and especially not recommended
to mix `get_val` measurements with the block read measuremens.

* `chan_set <chs> <en> <sngl> <rng>` -- set channel parameters.
  - `chs`  -- Channels: 01 to 16, multiple channels allowed.
  - `en`   -- Enable channels: 1 or 0.
  - `sngl` -- Single/Differential mode: 1 or 0.
  - `rng`  -- Input range, mV (see `ranges` command).

* `disable_all` -- Disable all channels. Do this before
  setting up channels if the device was in unknown state.

* `chan_get <ch>`  -- get channel parameters (`<ch> <en> <sngl> <rng>`,
  or `<ch> disabled` for disabled channels.

* `chan_get_n`  -- get number of enabled analog channels.

* `set_t <dt> <tconv>` -- Set timings.
  Parameter `dt` is measurement time [ms]. If N is number of enabled channels
  then dt should be: N*conv_t < dt <= 1000*N*conv_t. It makes sense
  only for block reads with multiple measurements which are not supported now.
  Set this to the smallest possible value (N*conv_t+1).
  Parameter `conv_t` is ADC conversion time for a single value [ms].
  Use longer times for more accurate measurements.

* `get` -- collect and return a single data block.
  Returns one value for each enabled channel.
  `chan_set` and `set_t` should be done before.
