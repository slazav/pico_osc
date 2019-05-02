## do test signals
## - correlated, non-correlates, partially correlated
all:
	for n in 0.0 0.01 0.1 0.2; do\
	  ../testsig_decay -N 150000 -D 1e-4 -F 2674 -T 1.23 -A 1.23 -n $$n > dat$$n.dat;\
	  ../sig_filter -f fit       dat$$n.dat > dat$$n.txt;\
	  ../sig_filter -f sfft_pnm dat$$n.dat -l -g KbmrRYW | pnmtopng > dat$$n.png;\
	done
	@echo "# Noise  F  1/T  Vpp Ph"
	@echo "# Theor: 2674 0.813008 1.23000 0"
	@for n in 0.0 0.01 0.1 0.2; do\
	  echo -n $$n;\
	  cat dat$$n.txt;\
	done

clean:
	rm -f *.dat *.txt *.png



