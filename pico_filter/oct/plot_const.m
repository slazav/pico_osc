function plot_const()

  # parameters
  A  = 0.892;       # signal amplitude
  n  = 0;           # noise amplitude
  f0  = 32.1891e3;  # frequency
  w0  = 2*pi*f0;
  p  = 0.22345;     # phase
  t0 = 0;           # tau

  T  = 5.7;
  N  = 4e5;      # number of points

  minf = 30e3;
  maxf = 35e3;


  # build model
  t = linspace(0,T,N);
  if (t0!=0)
    y = A*exp(-t/t0).*sin(w0*t+p) + n*(2*rand(size(t))-1);
  else
    y = A*sin(w0*t+p) + n*(2*rand(size(t))-1);
  end


  # make fft
  N = length(t);
  T = t(end)-t(1);
  f = linspace(0,(N-1)/T, N);
  z = 2*fft(y)/N;

#  kk = (exp(2*pi*f*i*t(end)) - exp(2*pi*f*i*t(1)));
#  z = z./k

  ### first approximation
  # find peak
  ii=find(f>minf & f<maxf);
  [~,mi] = max(abs(z(ii)));
  mi=ii(mi);
  # 3-point fit
  x1 = f(mi-1); y1=abs(z(mi-1));
  x2 = f(mi+1); y2=abs(z(mi+1));
  x3 = f(mi); y3=abs(z(mi));
  A = ((y1-y2)/(x1-x2) - (y2-y3)/(x2-x3))/(x1-x3);
  B = (y1-y2)/(x1-x2) - A*(x1+x2);
  C = y1 - A*x1^2 - B*x1;

  f0a = -B/(2*A);
  fma = -B^2/(4*A) + C;
  df0a = -sqrt(B^2-4*A*C)/A;
  t0a = 2/df0a/pi;
  Aa=fma/t0a;
  e=0;

  ### second approximation
  df = pi/t0a
  ii=find(f>f0-df & f<f0+df);

  # function for fit
  X = f(ii)-f0;
  Y = 1./z(ii);
  # weight
  w = z(ii).^4;
  s21 = sum(w.* X.^2);
  s11 = sum(w.* X);
  s01 = sum(w.* 1);
  s10 = sum(w.* X.*Y);
  s00 = sum(w.* Y);
  # solve 2x2 matrix
  AA = [s21 s11; s11 s01];
  BB = [s10 s00];
  CC= BB/AA;

  e = sum((CC(1)*X+CC(2)-Y).^2./Y)/length(X); # error
  Ab = 2*pi*i/CC(1);
  f0b = f0 - real(CC(2)/CC(1));
  t0b = -1/(2*pi*imag(CC(2)/CC(1)));

kk = (exp(2*pi*f0b*i*t(end) - t(end)/t0b) - exp(2*pi*f0b*i*t(1) - t(1)/t0b));

  find_figure("sig2"); clf; hold on

  #za = Aa./(i*2*pi*(f-f0a) + 1/t0a);
  zb = Ab./(i*2*pi*(f-f0b) + 1/t0b);

  if t0==0
    zt = A*exp(i*p)./(i*2*pi*(f-f0));
  else
    zt = A*exp(i*p)./(i*2*pi*(f-f0) + 1/t0);
  end

  plot(f, real(1./z), 'r.')
  plot(f, imag(1./z), 'b.')

  plot(f, real(1./zb), 'm-')
  plot(f, imag(1./zb), 'c-')

  plot(f, real(1./zt), 'k--')
  plot(f, imag(1./zt), 'k--')
  plot([minf, maxf], [0 0], 'k-')

  xlim(f0b + 10*[-1,1]);
  ylim([-1 1]*100);



end
