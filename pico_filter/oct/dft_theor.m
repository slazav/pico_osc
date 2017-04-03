function dft_theor()

  # calculate dft of a signal
  #   amp*exp(-t/tau).*sin(2*pi*fre*t + ph)
  function z = dft_calc(amp,tau,fre,ph, t)
    N = length(t);
    y = amp*exp(-t/tau).*sin(2*pi*fre*t + ph);
    z = 2*fft(y)/N;
  end


  # theory for the same thing
  function z = dft_theor(amp,tau,fre,ph, t)
    # frequency space
    N = length(t);
    T = t(end)-t(1);
    w = 2*pi*linspace(0,(N-1)/T, N);
    w0 = 2*pi*fre;
    w0 = w0*(1 + 1/N); # ?!

    # a complex linear function:
    lin = (w0-w)*1i - 1/tau;

    # constant factor which depends on boundary conditions:
    t1 = t(1); t2=t(end);
    bnd = exp(w0*1i*t2-t2/tau) - exp(w0*1i*t1-t1/tau);

    # phase and amplitude:
    pha = exp(1i*(ph-pi/2)) * amp/T;

    z = pha*bnd./lin;
  end

  # parameters
  amp = 0.892;
  fre = 32.18e3;
  tau = 0.521;
  ph  = 0.23;

  T  = 1.54;
  N  = 1e5;      # number of points

  minf = fre-1;
  maxf = fre+1;


  find_figure("dft_theor"); clf; hold on

  # calculate signal for different time grids
  for T = 2:0.01:2.5
    # build model
    t = linspace(0,T,N);
    f = linspace(0,(N-1)/T, N);
    z1 = dft_calc(amp,tau,fre,ph, t);
    z2 = dft_theor(amp,tau,fre,ph, t);

    ii = find(f>minf & f<maxf);
    if (0)
      plot(f(ii), real(1./z1(ii)), 'r.-')
      plot(f(ii), imag(1./z1(ii)), 'b.-')
      plot(f(ii), zeros(size(ii)), 'k-')
    else # plot ratio
      plot(f(ii), real(z1(ii)./z2(ii)), 'm-')
      plot(f(ii), imag(z1(ii)./z2(ii)), 'c-')
    end

  end



end
