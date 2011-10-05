#!/bin/sh
# ./plot-manual.sh 2 1024
# ./plot-manual.sh 3 4096
# ./plot-manual.sh 4 16368
gnuplot <<EOF
set terminal pdf
set output "$2"
set ylabel "Time (sec)"
set xlabel "Concurrent files"
plot "result-hdd.dat" every::1 using $1:xticlabels(1) w linespoints t "hdd", \
     "result-usb.dat" every::1 using $1:xticlabels(1) w linespoints t "usb", \
     "result-sdd.dat" every::1 using $1:xticlabels(1) w linespoints t "ssd"
EOF
