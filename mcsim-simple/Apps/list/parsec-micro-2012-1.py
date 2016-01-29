./kernels/canneal/ ../output/micro-2012/
  t-RBoL-epoch-canneal.out         t-RBoL-epoch.py -- ./canneal 256 15000 2000 400000.nets
  t-RBoL-canneal.out               t-RBoL.py -- ./canneal 256 15000 2000 400000.nets
  t-closed-canneal.out             t-closed.py -- ./canneal 256 15000 2000 400000.nets
  t-open-canneal.out               t-open.py -- ./canneal 256 15000 2000 400000.nets

./kernels/streamcluster/ ../output/micro-2012/
  t-RBoL-epoch-streamcluster.out   t-RBoL-epoch.py -- ./streamcluster 10 20 128 16384 16384 1000 none text1.txt 256
  t-RBoL-streamcluster.out         t-RBoL.py -- ./streamcluster 10 20 128 16384 16384 1000 none text1.txt 256
  t-closed-streamcluster.out       t-closed.py -- ./streamcluster 10 20 128 16384 16384 1000 none text1.txt 256
  t-open-streamcluster.out         t-open.py -- ./streamcluster 10 20 128 16384 16384 1000 none text1.txt 256

./apps/blackscholes/ ../output/micro-2012/
  t-RBoL-epoch-blackscholes.out    t-RBoL-epoch.py -- ./blackscholes 256 65536
  t-RBoL-blackscholes.out          t-RBoL.py -- ./blackscholes 256 65536
  t-closed-blackscholes.out        t-closed.py -- ./blackscholes 256 65536
  t-open-blackscholes.out          t-open.py -- ./blackscholes 256 65536

./apps/facesim/ ../output/micro-2012/
  t-RBoL-epoch-facesim.out         t-RBoL-epoch.py -- ./facesim -timing -threads 256
  t-RBoL-facesim.out               t-RBoL.py -- ./facesim -timing -threads 256
  t-closed-facesim.out             t-closed.py -- ./facesim -timing -threads 256
  t-open-facesim.out               t-open.py -- ./facesim -timing -threads 256

./apps/fluidanimate/ ../output/micro-2012/
  t-RBoL-epoch-fluidanimate.out    t-RBoL-epoch.py -- ./fluidanimate 256 5 in_300K.fluid text1.txt
  t-RBoL-fluidanimate.out          t-RBoL.py -- ./fluidanimate 256 5 in_300K.fluid text1.txt
  t-closed-fluidanimate.out        t-closed.py -- ./fluidanimate 256 5 in_300K.fluid text1.txt
  t-open-fluidanimate.out          t-open.py -- ./fluidanimate 256 5 in_300K.fluid text1.txt

./apps/swaptions/ ../output/micro-2012/
  t-RBoL-epoch-swaptions.out       t-RBoL-epoch.py -- ./swaptions -ns 64 -sm 20000 -nt 256
  t-RBoL-swaptions.out             t-RBoL.py -- ./swaptions -ns 64 -sm 20000 -nt 256
  t-closed-swaptions.out           t-closed.py -- ./swaptions -ns 64 -sm 20000 -nt 256
  t-open-swaptions.out             t-open.py -- ./swaptions -ns 64 -sm 20000 -nt 256

