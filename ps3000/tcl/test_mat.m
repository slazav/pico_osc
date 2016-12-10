function test_mat()
  % How to plot the recorded signal?
  dat = load('rec.mat')

  % Data is stored in short integers to reduce file size.
  % Don't forget to convert it to double!
  x = double(1:dat.len)*dat.Tscale+dat.Tshift;
  a = double(dat.Araw)*dat.Ascale;
  b = double(dat.Braw)*dat.Bscale;

  if dat.overload; fprintf('Overload!\n'); end

  figure; clf; hold on;
  plot(x, a, 'r-');
  plot(x, b, 'b-');
end
