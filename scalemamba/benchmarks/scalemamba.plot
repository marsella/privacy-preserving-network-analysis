set terminal epslatex color colortext standalone font ",14"

set size 1,1
set datafile separator ","
set style line 1 lc rgb 'gray' dt 2 lw 2

# RUNTIME
set output "scalemamba_runtime.tex"

set xlabel "Number of banks ($n$)"
set ylabel "Runtime (s)"

# compute best fit line
quad(t) = a*t**2 + b*t + c
fit quad(x) "scalemamba-EN.csv" using 1:2 via a,b,c

# compute best fit line with fixed parties
quadfix(t) = d*t**2 + e*t + f
fit quadfix(x) "scalemamba-fixed.csv" using 1:2 via d,e,f

# plot EN runtime
set key left top
set label at 17, 7000 tc ls 1 '1 hour'
set label at 17, 39400 tc ls 1 '10 hours'
set label at 17, 111400 tc ls 1 '30 hours'
plot [1:21] "scalemamba-EN.csv" using 1:2 pt 7 ps 1 lt rgb "blue" title "n computation parties", \
     [2:21] quad(x) lt rgb "blue" dt 2 title "", \
     [1:21] "scalemamba-fixed.csv" using 1:2 pt 7 ps 1 lt rgb "red" title "3 computation parties", \
     [2:21] quadfix(x) lt rgb "red" dt 2 title "", \
     [0:21] 3600 ls 1 title "", \
     [0:21] 36000 ls 1 title "", \
     [0:21] 108000 ls 1 title ""

# RAM
set output "scalemamba_ram.tex"
set xlabel "Number of banks ($n$)"
set ylabel "RAM usage (GB)"
unset label

# best fit line 
lin(t) = ar * t + br
fit lin(x) "scalemamba-EN.csv" using 1:3 via ar, br

# best fit line for fixed parties
linfix(t) = cr * t + dr
fit linfix(x) "scalemamba-fixed.csv" using 1:3 via cr,dr

set key left top
plot [1:21] "scalemamba-EN.csv" using 1:3 pt 7 ps 1 lt rgb "blue" title "n computation parties", \
     [2:21] lin(x) lt rgb "blue" dt 2 title "", \
     [1:21] "scalemamba-fixed.csv" using 1:3 pt 7 ps 1 lt rgb "red" title "3 computation parties", \
     [2:21] linfix(x) lt rgb "red" dt 2 title ""

###########################
# EGJ plots (for appendix)
set output "scalemamba_egj_runtime.tex"
set xlabel "Number of banks ($n$)"
set ylabel "Runtime (s)"

# compute EGJ best fit curve
quadegj(t) = ae*t**2 + be*t + ce
fit quadegj(x) "scalemamba-EGJ.csv" using 1:2 via ae,be,ce

set key left top
set label at 17, 7000 tc ls 1 '1 hour'
set label at 17, 39400 tc ls 1 '10 hours'
set label at 17, 111400 tc ls 1 '30 hours'
plot [1:21] "scalemamba-EN.csv" using 1:2 pt 7 ps 1 lt rgb "blue" title "Debt model", \
     [2:21] quad(x) lt rgb "blue" dt 2 title "", \
     [1:21] "scalemamba-EGJ.csv" using 1:2 pt 7 ps 1 lt rgb "red" title "Equity model", \
     [2:21] quadegj(x) lt rgb "red" dt 2 title "", \
     [0:21] 3600 ls 1 title "", \
     [0:21] 36000 ls 1 title "", \
     [0:21] 108000 ls 1 title ""


# make ram plots
unset label
set output "scalemamba_egj_ram.tex"
set xlabel "Number of banks ($n$)"
set ylabel "RAM usage (GB)"

# best fit line 
linegj(t) = are * t + bre
fit linegj(x) "scalemamba-EGJ.csv" using 1:3 via are, bre

set key left top
plot [1:21] "scalemamba-EN.csv" using 1:3 pt 7 ps 1 lt rgb "blue" title "Debt model", \
     [2:21] lin(x) lt rgb "blue" dt 2 title "", \
     [1:21] "scalemamba-EGJ.csv" using 1:3 pt 7 ps 1 lt rgb "red" title "Equity model", \
     [2:21] linegj(x) lt rgb "red" dt 2 title ""









