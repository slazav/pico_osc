## sig_pngfig script

Sometimes it is needed to extract some from spectrogram of
a signal.

I do this using `xfig` vector editor in the following way:

- The spectrogram is created by `sfft_pnm` filter, then
  it is converted to `png` using `sig_pnmtopng` program (to keep
  information about frequency and time ranges).

- A `fig` file is created by `sig_pngfig` script.

- Interesting features can be selected by drawing lines,
  points or rectangles (with optional single-line comments).
  By default only objects with depth 50 are considered as user marks.
  Use --mind --maxd to thange this.

- These features can be manipulated with `sig_pngfig` script

## Examples

Create/update fig file. If there are user marks in the old file they will be
transferred to the new one:

```
sig_pngfig -p <png> <fig>
```

Pring all user marks in a text form:
```
sig_pngfig -m get_data <fig> > <marks.txt>
```

Pring conversion coefficients for a given file:
```
sig_pngfig -m get_data <fig>
```

Add user marks to a fig file:
```
sig_pngfig -m put_data <fig> < <marks.txt>
```
