## fit_res

Program reads resonance data (time, w, X, Y) from stdin,
fits data with a Lorentzian function:
```
(X + iY) = (A + iB) + (C + iD)/(w0^2 - w^2 - iw*dw)
```

and prints a line with 14 values to stdout:
- time -- center of the time range
- f_error -- mean square difference between data and fit
- A, A_error, B, E_error -- shift in x and y component
- C, C_error, D, D_error -- driving force (not amplitude!
- w, w_error == resonance frequency (in Hz or rad/s dependind on input data)
- dw, dw_error -- width at 1/2 height of amplitude curve,
  or (appoximately) distance between dispersion minimum
  and maximum. If frequency data is in rad/s then df=2/tau,
  if data in Hz then df = pi/tau

Each parameter is followed by corresponding parameter error.

The program can be used as a filter in `graphene_filter` script
(see https://github.com/slazav/graphene/ )


Program uses libgsl library for fitting.

#### Overload detection

There is an option (turned on by default) for detecting overloaded signals.
Signals are fit twice: in a usual way and with points inside largest 5% of
the data range removed. The fit with smaller error is chosen.
