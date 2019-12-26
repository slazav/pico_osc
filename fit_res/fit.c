#include <stdlib.h>
#include <stdio.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_multifit_nlinear.h>
//#include <gsl/gsl_rng.h>
//#include <gsl/gsl_randist.h>

/********************************************************************/

// modified Gaussian example from
//https://www.gnu.org/software/gsl/doc/html/nls.html#c.gsl_multifit_nlinear_fdf

struct data {
  double *w;
  double *x;
  double *y;
  size_t n;
};

/* model function:

(X + iY) = (A + iB) + (C + iD)/(w0^2 - w^2 - iw*dw)

X(w) = A + (C*(w0^2-w^2) + D*w*dw) / ((w0^2-w^2)^2 + (w*dw)^2)
Y(w) = B + (D*(w0^2-w^2) - C*w*dw) / ((w0^2-w^2)^2 + (w*dw)^2)

*/

int
func_f (const gsl_vector * x, void *params, gsl_vector * f) {
  struct data *d = (struct data *) params;
  double A = gsl_vector_get(x, 0);
  double B = gsl_vector_get(x, 1);
  double C = gsl_vector_get(x, 2);
  double D = gsl_vector_get(x, 3);
  double w0 = gsl_vector_get(x, 4);
  double dw = gsl_vector_get(x, 5);
  size_t i;

  for (i = 0; i < d->n; ++i) {
    double wi = d->w[i];
    double Xi = d->x[i];
    double Yi = d->y[i];

    double wa = w0*w0 - wi*wi;
    double wb = wi*dw;
    double z = wa*wa + wb*wb;
    double X = A + (C*wa + D*wb)/z;
    double Y = B + (D*wa - C*wb)/z;

    gsl_vector_set(f, 2*i,   Xi - X);
    gsl_vector_set(f, 2*i+1, Yi - Y);
  }

  return GSL_SUCCESS;
}


// function derivatives
int
func_df (const gsl_vector * x, void *params, gsl_matrix * J) {
  struct data *d = (struct data *) params;
  double A = gsl_vector_get(x, 0);
  double B = gsl_vector_get(x, 1);
  double C = gsl_vector_get(x, 2);
  double D = gsl_vector_get(x, 3);
  double w0 = gsl_vector_get(x, 4);
  double dw = gsl_vector_get(x, 5);
  size_t i;

  for (i = 0; i < d->n; ++i) {
      double wi = d->w[i];
      double Xi = d->x[i];
      double Yi = d->y[i];

      double wa = w0*w0 - wi*wi;
      double wb = wi*dw;
      double z = wa*wa + wb*wb;

      gsl_matrix_set(J, 2*i, 0, -1);
      gsl_matrix_set(J, 2*i, 1, 0);
      gsl_matrix_set(J, 2*i, 2, -wa/z);
      gsl_matrix_set(J, 2*i, 3, -wb/z);
      gsl_matrix_set(J, 2*i, 4, -2*C*w0/z + (C*wa+D*wb)/z/z * 4*wa*w0);
      gsl_matrix_set(J, 2*i, 5, -D*wi/z   + (C*wa+D*wb)/z/z * 2*wb*wi);

      gsl_matrix_set(J, 2*i+1, 0, 0);
      gsl_matrix_set(J, 2*i+1, 1, -1);
      gsl_matrix_set(J, 2*i+1, 2, wb/z);
      gsl_matrix_set(J, 2*i+1, 3, -wa/z);
      gsl_matrix_set(J, 2*i+1, 4, -2*D*w0/z + (D*wa-C*wb)/z/z * 4*wa*w0);
      gsl_matrix_set(J, 2*i+1, 5, +C*wi/z   + (D*wa-C*wb)/z/z * 2*wb*wi);
  }

  return GSL_SUCCESS;
}

/*
// Additional derivatives for the accelerated method
// (see fdf_params.trs = gsl_multifit_nlinear_trs_lmaccel)
int
func_fvv (const gsl_vector * x, const gsl_vector * v,
          void *params, gsl_vector * fvv)
{
  struct data *d = (struct data *) params;
  double a = gsl_vector_get(x, 0);
  double b = gsl_vector_get(x, 1);
  double c = gsl_vector_get(x, 2);
  double va = gsl_vector_get(v, 0);
  double vb = gsl_vector_get(v, 1);
  double vc = gsl_vector_get(v, 2);
  size_t i;

  for (i = 0; i < d->n; ++i) {
      double ti = d->t[i];
      double zi = (ti - b) / c;
      double ei = exp(-0.5 * zi * zi);
      double Dab = -zi * ei / c;
      double Dac = -zi * zi * ei / c;
      double Dbb = a * ei / (c * c) * (1.0 - zi*zi);
      double Dbc = a * zi * ei / (c * c) * (2.0 - zi*zi);
      double Dcc = a * zi * zi * ei / (c * c) * (3.0 - zi*zi);
      double sum;

      sum = 2.0 * va * vb * Dab +
            2.0 * va * vc * Dac +
                  vb * vb * Dbb +
            2.0 * vb * vc * Dbc +
                  vc * vc * Dcc;

      gsl_vector_set(fvv, i, sum);
    }

  return GSL_SUCCESS;
}
*/

/********************************************************************/

void
callback(const size_t iter, void *params,
         const gsl_multifit_nlinear_workspace *w) {

  /*

  gsl_vector *f = gsl_multifit_nlinear_residual(w);
  gsl_vector *x = gsl_multifit_nlinear_position(w);
  double avratio = gsl_multifit_nlinear_avratio(w);
  double rcond;

  (void) params; // not used

  // compute reciprocal condition number of J(x)
  gsl_multifit_nlinear_rcond(&rcond, w);

  fprintf(stderr, "iter %2zu: a = %.4f, b = %.4f, c = %.4f, |a|/|v| = %.4f cond(J) = %8.4f, |f(x)| = %.4f\n",
          iter,
          gsl_vector_get(x, 0),
          gsl_vector_get(x, 1),
          gsl_vector_get(x, 2),
          avratio,
          1.0 / rcond,
          gsl_blas_dnrm2(f));
  */
}

double
solve_system(gsl_vector *x, gsl_vector *xe, gsl_multifit_nlinear_fdf *fdf,
             gsl_multifit_nlinear_parameters *params) {

  const gsl_multifit_nlinear_type *T = gsl_multifit_nlinear_trust;

  const size_t max_iter = 200;
  const double xtol = 1.0e-10;
  const double gtol = 1.0e-10;
  const double ftol = 1.0e-10;

  const size_t n = fdf->n;
  const size_t p = fdf->p;

  gsl_multifit_nlinear_workspace *work =
    gsl_multifit_nlinear_alloc(T, params, n, p);
  gsl_vector * f = gsl_multifit_nlinear_residual(work);
  gsl_vector * y = gsl_multifit_nlinear_position(work);


  int info;
  double chisq0, chisq, rcond;
  size_t i;


  /* initialize solver */
  gsl_multifit_nlinear_init(x, fdf, work);

  /* store initial cost */
  gsl_blas_ddot(f, f, &chisq0);

  /* iterate until convergence */
  gsl_multifit_nlinear_driver(max_iter, xtol, gtol, ftol,
                              callback, NULL, &info, work);

  /* store final cost */
  gsl_blas_ddot(f, f, &chisq);

  /* store cond(J(x)) */
  gsl_multifit_nlinear_rcond(&rcond, work);

  gsl_vector_memcpy(x, y);

  /* compute parameter errors (see first example in
     https://www.gnu.org/software/gsl/doc/html/nls.html ) */
  {
    gsl_matrix *covar = gsl_matrix_alloc (p, p);
    gsl_matrix *J = gsl_multifit_nlinear_jac(work);
    double c = sqrt(chisq / (n-p));
    gsl_multifit_nlinear_covar (J, 0.0, covar);

    for (i=0; i<p; i++)
      gsl_vector_set(xe, i, c*sqrt(gsl_matrix_get(covar,i,i)));

    gsl_matrix_free (covar);
  }

  /* print summary */
  /*
  fprintf(stderr, "summary from method '%s/%s'\n",
          gsl_multifit_nlinear_name(work),
          gsl_multifit_nlinear_trs_name(work));
  fprintf(stderr, "number of iterations: %zu\n",
          gsl_multifit_nlinear_niter(work));
  fprintf(stderr, "function evaluations: %zu\n", fdf->nevalf);
  fprintf(stderr, "Jacobian evaluations: %zu\n", fdf->nevaldf);
  fprintf(stderr, "reason for stopping: %s\n",
          (info == 1) ? "small step size" : "small gradient");
  fprintf(stderr, "initial |f(x)| = %f\n", sqrt(chisq0));
  fprintf(stderr, "final   |f(x)| = %f\n", sqrt(chisq));
  */

  gsl_multifit_nlinear_free(work);
  return sqrt(chisq/n);
}

/********************************************************************/
// Find initial conditions by some trivial assumptions.

void
fit_res_init (const size_t n,
         double * freq, double * real, double * imag,
         double pars[6]) {

  // A,B - in the middle between first and last point:
  double A = (real[0] + real[n-1])/2;
  double B = (imag[0] + imag[n-1])/2;

  // Find furthest point from (A,B),
  // It should be the resonance
  double dmax=0;
  size_t imax=0;
  for (size_t i = 0; i<n; i++) {
    double d = hypot(real[i]-A, imag[i]-B);
    if (d>dmax) {dmax=d; imax=i;}
  }
  double w0 = freq[imax];

  // Find dmax/sqrt(2) distance. It should be
  // maxima of dispersion component
  int ih1=0, ih2=n-1;
  for (size_t i = 0; i<imax; i++) {
    double d = hypot(real[i]-A, imag[i]-B);
    if (d>dmax/sqrt(2)) {ih1=i; break;}
  }
  for (size_t i = n-1; i>=imax; i--) {
    double d = hypot(real[i]-A, imag[i]-B);
    if (d>dmax/sqrt(2)) {ih2=i; break;}
  }
  double dw = (freq[ih2]-freq[ih1]);

  // amplitudes:
  double D =  freq[imax]*dw*(real[imax]-A);
  double C = -freq[imax]*dw*(imag[imax]-B);

  pars[0] = A;
  pars[1] = B;
  pars[2] = C;
  pars[3] = D;
  pars[4] = w0;
  pars[5] = dw;
}

/********************************************************************/
// Fit resonance with Lorentzian curve
double
fit_res (const size_t n,
         double * freq, double * real, double * imag,
         double pars[6], double pars_e[6]) {

  const size_t p = 6;    /* number of model parameters */

  gsl_vector *f = gsl_vector_alloc(2*n);
  gsl_vector *x  = gsl_vector_alloc(p);
  gsl_vector *xe = gsl_vector_alloc(p);
  gsl_multifit_nlinear_fdf fdf;
  gsl_multifit_nlinear_parameters fdf_params =
    gsl_multifit_nlinear_default_parameters();
  size_t i;
  struct data fit_data;

  fit_data.n = n;
  fit_data.w = freq;
  fit_data.x = real;
  fit_data.y = imag;

  /* define function to be minimized */
  fdf.f = func_f;
  fdf.df = func_df; // NULL;
  fdf.fvv = NULL; //func_fvv;
  fdf.n = 2*n;
  fdf.p = p;
  fdf.params = & fit_data;

  /* starting point */
  for (i=0; i<p; i++) gsl_vector_set(x, i, pars[i]);

//  fdf_params.trs = gsl_multifit_nlinear_trs_lmaccel;
  fdf_params.trs = gsl_multifit_nlinear_trs_lm;
  double res = solve_system(x, xe, &fdf, &fdf_params);

  for (i=0; i<p; i++) pars[i]  = gsl_vector_get(x, i);
  for (i=0; i<p; i++) pars_e[i] = gsl_vector_get(xe, i);

  gsl_vector_free(f);
  gsl_vector_free(x);
  gsl_vector_free(xe);

  return res;
}

