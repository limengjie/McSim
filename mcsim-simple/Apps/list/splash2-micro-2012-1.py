./kernels/cholesky/ ../output/micro-2012/
  t-open-cholesky.out             t-open.py -- ./CHOLESKY -p256 inputs/tk17.O

./kernels/fft/ ../output/micro-2012/
  t-open-fft.out                  t-open.py -- ./FFT -p256 -m20 -l6

./kernels/lu/contiguous_blocks/ ../output/micro-2012/
  t-open-lu.out                   t-open.py -- ./LU -p256

./kernels/radix/ ../output/micro-2012/
  t-open-radix.out                t-open.py -- ./RADIX -p256 -n8388608

./apps/barnes/ ../output/micro-2012/
  t-open-barnes.out               t-open.py -- ./BARNES < input.p256

./apps/fmm/ ../output/micro-2012/
  t-open-fmm.out                  t-open.py -- ./FMM < inputs/input.16384.p256

./apps/ocean/contiguous_partitions/ ../output/micro-2012/
  t-open-ocean.out                t-open.py -- ./OCEAN -p256

./apps/radiosity/ ../output/micro-2012/
  t-open-radiosity.out            t-open.py -- ./RADIOSITY -p 256 -batch

./apps/raytrace/ ../output/micro-2012/
  t-open-raytrace.out             t-open.py -- ./RAYTRACE -p256 -m128 inputs/car.env

./apps/volrend/ ../output/micro-2012/
  t-open-volrend.out              t-open.py -- ./VOLREND 256 inputs/head

./apps/water-spatial/ ../output/micro-2012/
  t-open-water-spatial.out        t-open.py -- ./WATER-SPATIAL < input.p256

./kernels/cholesky/ ../output/micro-2012/
  t-closed-cholesky.out           t-closed.py -- ./CHOLESKY -p256 inputs/tk17.O

./kernels/fft/ ../output/micro-2012/
  t-closed-fft.out                t-closed.py -- ./FFT -p256 -m20 -l6

./kernels/lu/contiguous_blocks/ ../output/micro-2012/
  t-closed-lu.out                 t-closed.py -- ./LU -p256

./kernels/radix/ ../output/micro-2012/
  t-closed-radix.out              t-closed.py -- ./RADIX -p256 -n8388608

./apps/barnes/ ../output/micro-2012/
  t-closed-barnes.out             t-closed.py -- ./BARNES < input.p256

./apps/fmm/ ../output/micro-2012/
  t-closed-fmm.out                t-closed.py -- ./FMM < inputs/input.16384.p256

./apps/ocean/contiguous_partitions/ ../output/micro-2012/
  t-closed-ocean.out              t-closed.py -- ./OCEAN -p256

./apps/radiosity/ ../output/micro-2012/
  t-closed-radiosity.out          t-closed.py -- ./RADIOSITY -p 256 -batch

./apps/raytrace/ ../output/micro-2012/
  t-closed-raytrace.out           t-closed.py -- ./RAYTRACE -p256 -m128 inputs/car.env

./apps/volrend/ ../output/micro-2012/
  t-closed-volrend.out            t-closed.py -- ./VOLREND 256 inputs/head

./apps/water-spatial/ ../output/micro-2012/
  t-closed-water-spatial.out      t-closed.py -- ./WATER-SPATIAL < input.p256

./kernels/cholesky/ ../output/micro-2012/
  t-RBoL-cholesky.out             t-RBoL.py -- ./CHOLESKY -p256 inputs/tk17.O

./kernels/fft/ ../output/micro-2012/
  t-RBoL-fft.out                  t-RBoL.py -- ./FFT -p256 -m20 -l6

./kernels/lu/contiguous_blocks/ ../output/micro-2012/
  t-RBoL-lu.out                   t-RBoL.py -- ./LU -p256

./kernels/radix/ ../output/micro-2012/
  t-RBoL-radix.out                t-RBoL.py -- ./RADIX -p256 -n8388608

./apps/barnes/ ../output/micro-2012/
  t-RBoL-barnes.out               t-RBoL.py -- ./BARNES < input.p256

./apps/fmm/ ../output/micro-2012/
  t-RBoL-fmm.out                  t-RBoL.py -- ./FMM < inputs/input.16384.p256

./apps/ocean/contiguous_partitions/ ../output/micro-2012/
  t-RBoL-ocean.out                t-RBoL.py -- ./OCEAN -p256

./apps/radiosity/ ../output/micro-2012/
  t-RBoL-radiosity.out            t-RBoL.py -- ./RADIOSITY -p 256 -batch

./apps/raytrace/ ../output/micro-2012/
  t-RBoL-raytrace.out             t-RBoL.py -- ./RAYTRACE -p256 -m128 inputs/car.env

./apps/volrend/ ../output/micro-2012/
  t-RBoL-volrend.out              t-RBoL.py -- ./VOLREND 256 inputs/head

./apps/water-spatial/ ../output/micro-2012/
  t-RBoL-water-spatial.out        t-RBoL.py -- ./WATER-SPATIAL < input.p256

./kernels/cholesky/ ../output/micro-2012/
  t-TBoL-epoch-cholesky.out       t-TBoL-epoch.py -- ./CHOLESKY -p256 inputs/tk17.O

./kernels/fft/ ../output/micro-2012/
  t-TBoL-epoch-fft.out            t-TBoL-epoch.py -- ./FFT -p256 -m20 -l6

./kernels/lu/contiguous_blocks/ ../output/micro-2012/
  t-TBoL-epoch-lu.out             t-TBoL-epoch.py -- ./LU -p256

./kernels/radix/ ../output/micro-2012/
  t-TBoL-epoch-radix.out          t-TBoL-epoch.py -- ./RADIX -p256 -n8388608

./apps/barnes/ ../output/micro-2012/
  t-TBoL-epoch-barnes.out         t-TBoL-epoch.py -- ./BARNES < input.p256

./apps/fmm/ ../output/micro-2012/
  t-TBoL-epoch-fmm.out            t-TBoL-epoch.py -- ./FMM < inputs/input.16384.p256

./apps/ocean/contiguous_partitions/ ../output/micro-2012/
  t-TBoL-epoch-ocean.out          t-TBoL-epoch.py -- ./OCEAN -p256

./apps/radiosity/ ../output/micro-2012/
  t-TBoL-epoch-radiosity.out      t-TBoL-epoch.py -- ./RADIOSITY -p 256 -batch

./apps/raytrace/ ../output/micro-2012/
  t-TBoL-epoch-raytrace.out       t-TBoL-epoch.py -- ./RAYTRACE -p256 -m128 inputs/car.env

./apps/volrend/ ../output/micro-2012/
  t-TBoL-epoch-volrend.out        t-TBoL-epoch.py -- ./VOLREND 256 inputs/head

./apps/water-spatial/ ../output/micro-2012/
  t-TBoL-epoch-water-spatial.out  t-TBoL-epoch.py -- ./WATER-SPATIAL < input.p256

