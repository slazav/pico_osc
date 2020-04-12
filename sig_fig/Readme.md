Sometimes it is needed to extract some from spectrogram of
a signal.

I do this using `xfig` vector editor in the following way:

- The spectrogram is created by `sfft_pnm` filter, then
  it is converted to `png` using `sig_pnmtopng` program (to keep
  information about frequency and time ranges).

- A `fig` file is created by `sig_pngfig` script.

- Interesting features can be selected by drawing lines,
  points or rectangles (with optional single-line comments).

- These features are extracted using `sig_figdat` script.
