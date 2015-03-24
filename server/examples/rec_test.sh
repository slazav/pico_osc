#!/bin/sh

## geterator should be connected to A channel to provite triggering for this test

## set channels:
wget -q -O - -S "localhost:8081/set_chan?channel=A&coupling=DC&range=0.4"
wget -q -O - -S "localhost:8081/set_chan?channel=B&coupling=DC&range=0.4"

## set generator: 5 cycles, software triggering
wget -q -O - -S "localhost:8081/set_gen?volt=0.35&f1=30&f2=30&cycles=5&trig_src=soft"

## set oscilloscope trigger, source A channel
wget -q -O - -S "localhost:8081/set_trig?src=A&dir=rising&enable=1&thr=100"

## run recording, do generator triggering
wget -q -O rec.dat -S "localhost:8081/rec_block?rate=3000&pretrig=0.2&time=0.2&triggen"
