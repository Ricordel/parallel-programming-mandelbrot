CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -c -O2 -DNDEBUG
LD = gcc
LDFLAGS = -lm

COMMON_HEADERS = src/dbg.h src/mandelbrot.h

all: mandelbrot_serial

dev: CFLAGS = -g -std=c99 -Wall -Wextra -O0 -c
dev: all

mandelbrot_serial: bin/mandelbrot_serial.o bin/mandelbrot.o
	$(LD) $(LDFLAGS) $^ -o $@

bin/%.o: src/%.c $(COMMON_HEADERS)
	$(CC) $(CFLAGS) $< -o $@



clean:
	rm -f bin/*
	rm -f mandelbrot_serial
	rm -f *.pyc


.PHONY: clean
