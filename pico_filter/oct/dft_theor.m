function dft_theor()

  # DFT [ A*exp(-t/t0).*sin(w0*t+p) ] = ?



  # parameters
  A  = 0.892;       # signal amplitude
  n  = 0;           # noise amplitude
  f0  = 32.18e3;    # frequency
  w0  = 2*pi*f0;
  p  = 0.23;        # phase
  t0 = 0.521;       # tau

  T  = 1.54;
  N  = 2^18;      # number of points

  minf = f0-4;
  maxf = f0+4;


  figure; clf; hold on

  for T = 2:0.01:2.03

    # build model
    t = linspace(0,T,N);
    y = A*exp(-t/t0).*sin(w0*t+p) + n*(2*rand(size(t))-1);

    # make fft
    f = linspace(0,(N-1)/T, N);
    w = 2*pi*f;
    z = 2*fft(y)/N;

    ii = find(f>minf & f<maxf);

    # constant scaling factor which depends on boundary conditions 
    t1 = 0;
    t2 = T;
    w0 = w0*(1 + 1/N);
    kk = exp(1i*w0*t2 - t2/t0) - exp(1i*w0*t1-t1/t0);

    # analitic function for 1/dft
    u = ( (w0-w(ii))*1i - 1/t0)/kk * exp(1i*(pi/2-p)) * T/A;

    plot(f(ii), real(z(ii).*u), 'm.-')
    plot(f(ii), imag(z(ii).*u), 'c.-')

  end



end
