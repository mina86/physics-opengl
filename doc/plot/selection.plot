set terminal postscript eps color

set pointsize .1

set xrange [0:1000000]
set yrange [650:*]

set title "Selekcja"

set style data steps

plot 'out/defaults.dat'              using 1:2 title 'trywialna', \
     'out/select-proportional.dat'   using 1:2 title 'proporcjonalna', \
     'out/select-random-uniform.dat' using 1:2 title 'losowa', \
     'out/select-tournament-2.dat'   using 1:2 title 'turniejowa'
