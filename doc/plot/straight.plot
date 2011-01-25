set terminal postscript eps color

set pointsize .1

set xrange [0:1000000]
set yrange [650:*]

set title "Prosta sukcesja"

plot 'out/succ-straight.dat' using 1:2 notitle
