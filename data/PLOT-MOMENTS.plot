reset 

set terminal postscript eps enhanced colour size 4.0, 3.0 "Helvetica" 24
set output './EPS-MOMENTS.eps'

data = './moments'
dataIDL = './IDL/ep5-ra4-ju2.00000-ln16384-nP1024-eB0-tB1000.00-tM1.00000e-07-v0.ngp'


set log xy
set mxtics 10
set mytics 10

set format x "10^{%L}"
set format y "10^{%L}"

f(x) = x

set key top left

set xlabel "t / t_B"
set ylabel "MSD / R^2"

plot data u 1:($3-$2**2) w l t "Sim C++", dataIDL u 1:($3-$2**2) w l t "Sim IDL", f(x) t "Linear"
