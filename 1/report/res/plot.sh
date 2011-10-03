#!/bin/sh
gnuplot <<EOF
set terminal pdf
set output "$2"
set ylabel "Time (sec)"
set xlabel "Concurrent files"
plot "$1" using 2:xticlabels(1) w linespoints t columnheader(2), \
"$1" using 3:xticlabels(1) w linespoints t columnheader(3), \
"$1" using 4:xticlabels(1) w linespoints t columnheader(4)
EOF
