#!/bin/sh

# Reproduction of the figure from the lecture notes
mpirun -np 4 ./mandelbrot_mpi -o gallery/lecture_notes.img -w 2048 -h 2048 -m 255 -t 2.0 -c -2.0,-2.0,2.0,2.0 && ./make_png.py gallery/lecture_notes.img gallery/lecture_notes.png 11

# The whole set in 16/9
mpirun -np 4 ./mandelbrot_mpi -o gallery/complete_16_9.img -w 4000 -h 2250 -m 80 -t 3.0 -c -2.5,-1.125,1.5,1.125 && ./make_png.py gallery/complete_16_9.img gallery/complete_16_9.png 4

# Zoom on a top-right detail
mpirun -np 4 ./mandelbrot_mpi -o gallery/zoom_top_right.img -w 3000 -h 3000 -m 100 -t 3.0 -c 0.2,0.45,0.47,0.72 && ./make_png.py gallery/zoom_top_right.img gallery/zoom_top_right.png 8

# Zoom on the left part of the image
mpirun -np 4 ./mandelbrot_mpi -o gallery/zoom_left.img -w 3000 -h 3000 -m 80 -t 3.0 -c -1.7,-0.5,-0.7,0.5 && ./make_png.py gallery/zoom_left.img gallery/zoom_left.png 10
