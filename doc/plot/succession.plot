set terminal postscript eps color

set pointsize .1

set xrange [0:1000000]
set yrange [650:1000]

set title "Sukcesja"

plot 'out/defaults.dat'            using 1:2 title 'elitarna', \
     'out/succ-union-of-elite.dat' using 1:2 title 'unia elit'
