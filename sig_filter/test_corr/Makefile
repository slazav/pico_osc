## do test signals
## - correlated, non-correlates, partially correlated
all:
	for c in 0.0 0.5 1.0; do\
	  ../testsig_noise -N 150000 -D 1e-4 -A 1 -c $$c > dat$$c.dat;\
	  ../sig_filter -f fft_pow      dat$$c.dat > dat$$c.nc.txt;\
	  ../sig_filter -f fft_pow_corr dat$$c.dat > dat$$c.co.txt;\
	done

clean:
	rm -f *.dat *.txt



