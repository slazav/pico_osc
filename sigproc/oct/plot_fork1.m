function plot_fork1()

  # parameters
  a  = 1;          # signal amplitude
  n  = 2;      # noise amplitude
  f0  = 32.188e3;  # frequency
  w0  = 2*pi*f0;
  p  = 2;          # phase
  t0 = 1;        # tau


  T  = 5*t0;
  N  = 5e5;      # fix number of points

  minf = 30e3;
  maxf = 35e3;


    # build model
    t = linspace(0,T,N);
    y = a*exp(-t/t0).*sin(w0*t+p) + n*(2*rand(size(t))-1);

    # make fft
    N = length(t);
    T = t(end)-t(1);
    f = linspace(0,(N-1)/T, N);
    z = fft(y);

    # fit
    ii=find(f>minf & f<maxf);
    [A1, t0a, f0a, e1] = fit_signal1(f(ii),z(ii));
    [A2, t0b, f0b, e2] = fit_signal2(f(ii),z(ii));

    t0a
    t0b


end
