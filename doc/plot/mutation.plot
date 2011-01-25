set terminal postscript eps color

set pointsize .1

set xrange [0:1000000]
set yrange [650:1500]

set title "Mutacja"

plot 'out/defaults.dat'  using 1:2 title 'P=0.7, sigma=0.1', \
     'out/mut-0.7,1.dat' using 1:2 title 'P=0.7, sigma=1.0', \
     'out/mut-0.3,1.dat' using 1:2 title 'P=0.3, sigma=1.0'
