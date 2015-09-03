set terminal x11

set title "CMSSMNoFV particle spectrum"
set ylabel "mass / GeV"
unset key
unset bars

if (!exists("filename")) filename='CMSSMNoFV_spectrum.dat'

plot filename using 1:2:(0.4):xtic(3) with xerrorbars pointtype 0 linecolor rgb "black"