all: noise.txt fork2.png fork2.txt lockin1.txt


noise.txt: noise.dat
	../pico_filter/pico_filter -f fft_pow_lavr noise.dat > noise.txt

fork2.png: fork2.dat
	../pico_filter/pico_filter -f pnm fork2.dat | pnmtopng > fork2.png

fork2.txt: fork2.dat
	../pico_filter/pico_filter -f txt -T -0.001 -U 0.004 fork2.dat > fork2.txt

lockin1.txt: lockin1.dat
	../pico_filter/pico_filter -f txt -U 0.001 lockin1.dat > lockin1.txt
