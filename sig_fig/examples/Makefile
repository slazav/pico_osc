#all: sig1.fig sig2.fig
all: sig1.txt sig2.txt


# sigf files are FFT of signals with cropped frequency range (to reduce file size).


# create png files
sig1.png: sig1.sigf
	sig_filter -f sfft_pnm $< -F 0 -G 1.8e3 -w 10000 -W 640 -H 1280  | sig_pnmtopng > $@

sig2.png: sig2.sigf
	sig_filter -f sfft_pnm $< -l -F 42955.99 -G 45956.00 -w 50000 -W 1280 -H 640  | sig_pnmtopng > $@


# create fig files
sig1.fig: sig1.png
	sig_pngfig $< $@

sig2.fig: sig2.png
	sig_pngfig --df 44462.0415438 $< $@

# information from fig files:
%.txt: %.fig
	sig_figdat $< > $@
