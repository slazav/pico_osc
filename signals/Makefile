all: noise.txt fork2.png fork2s.png fork2.txt lockin1.txt


noise.txt: noise.dat
	../sig_filter/sig_filter -f fft_pow_lavr noise.dat > noise.txt
	../sig_filter/sig_filter -T 0.03 -f fft_pow_lavr noise.dat > noise1.txt

fork2.png: fork2.dat
	../sig_filter/sig_filter -f pnm fork2.dat | pnmtopng > fork2.png

fork2s.png: fork2.dat
	../sig_filter/sig_filter -f sfft_pnm fork2.dat | pnmtopng > fork2s.png

fork2.txt: fork2.dat
	../sig_filter/sig_filter -f txt -T -0.001 -U 0.004 fork2.dat > fork2.txt

lockin1.txt: lockin1.dat
	../sig_filter/sig_filter -f txt -U 0.001 lockin1.dat > lockin1.txt

clean:
	rm -f *.txt *.png
