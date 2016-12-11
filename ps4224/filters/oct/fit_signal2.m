function [A, t0, f0, e] = fit_signal2(f,z)
  do_plot = 0;

  [Aa, t0a, f0a] = fit_signal2a(f,z, 32000,5000);
  [Ab t0b, f0b, e] = fit_signal2b(f,z, f0a,4/t0a);
  [A, t0, f0, e] = fit_signal2b(f,z, f0b,4/t0b);

  if (do_plot)
    figure; clf; hold on

    f1 = f0b + linspace(-5/t0b,+5/t0b, 1000);
    z1 = Aa./(i*2*pi*(f1-f0a) - 1/t0a);
    z2 = Ab./(i*2*pi*(f1-f0b) + 1/t0b);

#    plot(f(ii), real(1./z(ii)), 'r.-')
#    plot(f(ii), imag(1./z(ii)), 'b.-')
#    plot(f1, real(CC(1)*(f1-f0a)+CC(2)), 'm-')
#    plot(f1, imag(CC(1)*(f1-f0a)+CC(2)), 'c-')

#    plot(f(ii), real(z(ii)), 'r.-')
#    plot(f(ii), imag(z(ii)), 'b.-')

#    plot(f1, real(A./(2*pi*i*(f1-f0b)-1/t0b)), 'm-')
#    plot(f1, imag(A./(2*pi*i*(f1-f0b)-1/t0b)), 'c-')

#    plot(f1, real(z2), 'r-')
#    plot(f1, imag(z2), 'b-')

    plot(f, abs(z), 'r.-')
    plot(f1, abs(z1), 'b-')
    plot(f1, abs(z2), 'b-')
    xlim([-5,5]/t0b+f0b)

  end
end


# first approximation
function [A, t0a, f0a] = fit_signal2a(f,z, f0,df)
  # find peak
  ii=find(f>f0-df/2 & f<f0+df/2);
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
  A=fma/t0a;
  e=0;
end

# second approximation
function [A, t0b, f0b, e] = fit_signal2b(f,z, f0,df)
  ii=find(f>f0-df/2 & f<f0+df/2);
  # exact fit (real(1./z), imag(1./z)   -- todo 1/|Y| weight)
  Y0 = max(z(ii));
  X = f(ii)-f0;
  Y = Y0./z(ii);
  s21 = sum(X.^2./abs(Y).^4);
  s11 = sum(X./abs(Y).^4);
  s01 = sum(1./abs(Y).^4);
  s10 = sum(X.*Y./abs(Y).^4);
  s00 = sum(Y./abs(Y).^4);

  # solve 2x2 matrix
  AA = [s21 s11; s11 s01];
  BB = [s10 s00];
  CC= BB/AA;

  e = sum((CC(1)*X+CC(2)-Y).^2./Y)/length(X);
  A = 2*pi*i/CC(1)*Y0;
  f0b = f0 - real(CC(2)/CC(1));
  t0b = -1/(2*pi*imag(CC(2)/CC(1)));
end

