#ifndef FIT_RES_H
#define FIT_RES_H


/*
Model function:

(X + iY) = (A + iB) + (C + iD)/(w0^2 - w^2 - iw*dw)

X(w) = A + (C*(w0^2-w^2) + D*w*dw) / ((w0^2-w^2)^2 + (w*dw)^2)
Y(w) = B + (D*(w0^2-w^2) - C*w*dw) / ((w0^2-w^2)^2 + (w*dw)^2)
*/

/*
Find initial conditions by some trivial assumptions.
Arguments:
  n   - number of points
  freq - frequency data [0..n-1]
  real - X (real part) of the data [0..n-1]
  imag - Y (imag part) of the data [0..n-1]
  pars - array of size 6, fit parameters to be returned:
         base_real, base_imag, amp_real, amp_imag, res_freq, res_width
*/
void fit_res_init (const size_t n,
         double * freq, double * real, double * imag,
         double pars[6]);


/*
Fit resonance with Lorentzian curve
Arguments:
  n   - number of points
  freq - frequency data [0..n-1]
  real - X (real part) of the data [0..n-1]
  imag - Y (imag part) of the data [0..n-1]
  pars - array of size 6, fit parameters:
         base_real, base_imag, amp_real, amp_imag, res_freq, res_width
         On input initial values (e.g from fit_res_init) should be provided,
         On output parameters are changed to new values.
  pars_e -- parameter errors
Return value:
  mean square difference of the fitted function
*/

double fit_res (const size_t n,
                double * freq, double * real, double * imag,
                double pars[6], double pars_e[6]);

#endif
