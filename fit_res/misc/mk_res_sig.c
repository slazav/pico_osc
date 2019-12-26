#include <stdlib.h>
#include <stdio.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

// Make signal sutable for fit_res.
// Signal contains gaussian noise with sigma=0.1
// Same value should appear in f_error value
// of res_fit output

int
main (void) {
  const size_t n = 300;  /* number of data points */

  double A = 1.1;
  double B = 0.1;
  double C = 3000.2;
  double D = 2.2;
  double w0 = 1023;
  double dw = 11;

  double noise_s = 0.1;

  /* generate synthetic data with noise */
  {
    int i;
    gsl_rng * r;
    const gsl_rng_type * T = gsl_rng_default;
    gsl_rng_env_setup ();
    r = gsl_rng_alloc (T);

    for (i = 0; i < n; ++i) {

      double wi = w0 + 3*dw*( (double)i / (double) n - 0.5);
      double wa = w0*w0 - wi*wi;
      double wb = wi*dw;
      double z = wa*wa + wb*wb;
      double X = A + (C*wa + D*wb)/z;
      double Y = B + (D*wa - C*wb)/z;

      double Nx = gsl_ran_gaussian (r, noise_s);
      double Ny = gsl_ran_gaussian (r, noise_s);

      printf("%d %e %e %e\n", i, wi, X + Nx, Y+Ny);
    }
    gsl_rng_free(r);

  }

  return 0;
}
