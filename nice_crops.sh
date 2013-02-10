#!/bin/sh

images_dir=images


# The whole image reasonably zoomed
./mandelbrot_serial -o $images_dir/complete_square.img -w 3000 -h 3000 -m 40 -t 3.0 -c -2.2,-1.5,0.8,1.5

# Full 16/9
./mandelbrot_serial -o $images_dir/complete_16_9.img -w 4000 -h 2250 -m 40 -t 3.0 -c -2.5,-1.125,1.5,1.125

# A zoom on the front
./mandelbrot_serial -o $images_dir/zoom_on_front.img -w 3000 -h 3000 -m 40 -t 3.0 -c -1.7,-0.5,-0.7,0.5

# Close zoom top left
mpirun -np 4 ./mandelbrot_mpi -o out.img -w 3000 -h 3000 -m 40 -t 3.0 -c 0.2,0.45,0.47,0.72


# Nice with the cosine
mpirun -np 4 ./mandelbrot_mpi -o test.img -w 4000 -h 2500 -m 30 -t 10 -c -4,-2.25,4,2.25

# Nice with tricorne
mpirun -np 4 ./mandelbrot_mpi -o test.img -w 5000 -h 5000 -m 50 -t 3.0 -c -2.5,-2,1.5,2
