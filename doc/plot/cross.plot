set terminal postscript eps color

set pointsize .1

set xrange [0:1000000]
set yrange [650:*]

set title "Krzyzowanie"

set style data steps

plot 'out/defaults.dat'     using 1:2 title 'Srednia, P=0.1', \
     'out/cross-0.7.dat'    using 1:2 title 'Srednia, P=0.7', \
     'out/cross-random.dat' using 1:2 title 'Losowa, P=0.1', \
     'out/cross-interx.dat' using 1:2 title 'Zamiana, P=0.1'
