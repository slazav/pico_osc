function [A, t0b, f0b, e] = fit_signal1(f,z)
  do_plot = 0;

  minf = 20e3;
  maxf = 45e3;

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

  # first approximation
  f0a = -B/(2*A);
  fma = -B^2/(4*A) + C;
  df0a = -sqrt(B^2-4*A*(C-fma/2))/A;
  t0a = 2/df0a/pi;

  # exact fit (fit Y=1/z^2 with 1/Y^2 weight)
  ii=find(f>f0a-2/t0a & f<f0a+2/t0a);

  X = f(ii)-f0a;
  Y = 1./(abs(z(ii))/fma).^2;
  s42 = sum(X.^4./Y.^2);
  s32 = sum(X.^3./Y.^2);
  s22 = sum(X.^2./Y.^2);
  s12 = sum(X./Y.^2);
  s02 = sum(1./Y.^2);
  s21 = sum(X.^2./Y);
  s11 = sum(X./Y);
  s01 = sum(1./Y);

  # solve 3x3 matrix
  AA = [s42 s32 s22; s32 s22 s12; s22 s12 s02];
  BB = [s21 s11 s01];
  CC= BB/AA;

  e = sum(((CC(1)*X.^2+CC(2)*X+CC(3)-Y)./Y).^2)/length(X);

  A = 2*pi*fma/sqrt(CC(1));
  f0b = abs( f0a - CC(2)/(2*CC(1)) );
  t0b = abs( 1/2/pi/sqrt(CC(3)/CC(1) - (CC(2)/(2*CC(1)))^2) );

  if (do_plot)
    figure; clf; hold on
    #  plot(X,Y, 'r.-')
    #  plot(X, CC(1)*X.^2+CC(2)*X+CC(3), 'b-')

    #  plot(f(ii), abs(z(ii)), 'r.-')
    #  plot(f(ii), A./sqrt( (f(ii)-f0b).^2 + 1/t0b^2), 'b-')

    f1 = f0b + linspace(-5/t0b,+5/t0b, 1000);
    z1 = A./(i*2*pi*(f1-f0b) + 1/t0b);

    plot(f(ii), abs(z(ii)), 'r.-')
    plot(f1, abs(z1), 'b-')
    xlim([-5,5]/t0b+f0b)
  end
end

