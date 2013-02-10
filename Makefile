CC = gcc -c
MPICC = mpicc -c
CFLAGS = -std=c99 -Wall -Wextra -O2 -DNDEBUG
LD = gcc
MPILD = mpicc
LDFLAGS = -lm

COMMON_HEADERS = src/dbg.h src/mandelbrot.h src/option_parser.h

all: mandelbrot_serial mandelbrot_mpi

dev: CFLAGS = -g -std=c99 -Wall -Wextra -O0
dev: all

mandelbrot_serial: bin/mandelbrot_serial.o bin/mandelbrot.o bin/option_parser.o
	$(LD) $^ $(LDFLAGS) -o $@

mandelbrot_mpi: bin/mandelbrot_mpi.o bin/mandelbrot.o bin/option_parser.o
	$(MPILD) $^ $(LDFLAGS) -o $@

bin/%_mpi.o: src/%_mpi.c $(COMMON_HEADERS)
	$(MPICC) $(CFLAGS) $< -o $@

bin/%.o: src/%.c $(COMMON_HEADERS)
	$(CC) $(CFLAGS) $< -o $@


clean:
	rm -f bin/*
	rm -f mandelbrot_serial mandelbrot_mpi
	rm -f *.pyc


.PHONY: clean
