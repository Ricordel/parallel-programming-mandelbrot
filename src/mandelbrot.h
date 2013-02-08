#ifndef __MANDELBROT_H__
#define __MANDELBROT_H__



#include <stdint.h>
#include <complex.h> /* NOTE: needs a C99 compliant compiler ! */




/**
 * A color is simply a number (255 colors will be enough)
 */
typedef uint8_t color_t;



/**
 * Allocate a 2-dimensional array of colors.
 * We want to index our image [x][y] with x on the horizontal direction,
 * so x must be able to go from 0 to nCols - 1, and y from 0 to nRows - 1
 */
color_t ** allocate_image(uint32_t nCols, uint32_t nRows);


/**
 * Type for an iteration function, applied to the current value
 * of the sequence and to the point
 */
typedef double complex (mandelbrot_function_t(double complex z_n, double complex point));





int compute_window(color_t **image, double startX, double startY, double stepX,
                   double stepY, uint32_t width, uint32_t height, double threshold,
                   uint32_t maxIter, mandelbrot_function_t func);







/**
 * Save an image as an ASCII file (for portability). Print the dimensions of
 * the image on the first line, to ease further reading, then the values line by line.
 * NB: the coordinate system in the image has the Y-axis reversed compared to the
 * one on the classical complex plan. So we must invert it.
 */
int save_image(color_t **image, uint32_t width, uint32_t height, FILE *outFile);





/**** Command-line options *****/
struct prog_options {
        double threshold;
        uint32_t width;
        uint32_t height;
        uint32_t maxIter;

        double startX;
        double startY;
        double endX;
        double endY;

        char *outFileName;
};



int parse_options(struct prog_options *pProgOptions, int argc, char **argv);

double complex classic_mandelbrot(double complex z_n, double complex point);

#endif /* end of include guard: __MANDELBROT_H__ */
