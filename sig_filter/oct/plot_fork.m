function plot_fork()

  # parameters
  a  = 1;          # signal amplitude
  n  = 0.001;      # noise amplitude
  f0  = 32.188e3;  # frequency
  w0  = 2*pi*f0;
  p  = 2;          # phase
  t0 = 0.563;        # tau


  T  = 1*t0;
  N  = 1e6;      # fix number of points

  minf = 25e3;
  maxf = 45e3;


  n=0:0.02:2;
  for j=1:length(n)
    # build model
    t = linspace(0,T,N);
    y = a*exp(-t/t0).*sin(w0*t+p) + n(j)*(2*rand(size(t))-1);

    # make fft
    N = length(t);
    T = t(end)-t(1);
    f = linspace(0,(N-1)/T, N);
    z = fft(y);

    # fit
    ii=find(f>minf & f<maxf);
    [A1(j), t0b1(j), f0b1(j), e1(j)] = fit_signal1(f(ii),z(ii));
    [A2(j), t0b2(j), f0b2(j), e2(j)] = fit_signal2(f(ii),z(ii));
  end

  figure; clf; hold on
#  plot(n, t0b1, 'r.-');
  plot(n, t0b2/t0, 'b.-');




end
