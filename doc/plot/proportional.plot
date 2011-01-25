set terminal postscript eps color

set pointsize .1

set xrange [0:1000000]
set yrange [650:*]

set title "Selekcja proporcjonalna"

plot 'out/select-proportional.dat' using 1:2 title 'Populacja:  20', \
     'out/select-proportional+pop-100.dat' using 1:2 title 'Populacja: 100', \
     'out/select-proportional+pop-500.dat' using 1:2 title 'Populacja: 500'
