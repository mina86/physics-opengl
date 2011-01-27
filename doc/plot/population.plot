set terminal postscript eps color

set pointsize .1

set xrange [0:1000000]
set yrange [650:1000]

set title "Ustawienia domyslne"

set style data steps

plot 'out/defaults.dat' using 1:2 title 'Populacja:  20', \
     'out/pop-100.dat'  using 1:2 title 'Populacja: 100', \
     'out/pop-500.dat'  using 1:2 title 'Populacja: 500'
