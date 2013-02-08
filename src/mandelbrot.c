#include <stdlib.h>
#include <stdint.h>
#include <complex.h> /* NOTE: needs a C99 compliant compiler ! */
#include <getopt.h>
#include <math.h>

#include "dbg.h"
#include "mandelbrot.h"




/**
 * Allocate a 2-dimensional array of colors.
 * We want to index our image [x][y] with x on the horizontal direction,
 * so x must be able to go from 0 to nCols - 1, and y from 0 to nRows - 1
 */
color_t * allocate_image(uint32_t nCols, uint32_t nRows)
{
        color_t * image = (color_t *) calloc(nCols * nRows, sizeof(color_t));
        check_mem(image);

        return image;
}


/**
 * Type for an iteration function, applied to the current value
 * of the sequence and to the point
 */
typedef double complex (mandelbrot_function_t(double complex z_n, double complex point));




/**
 * Compute the color of a pixel using func to apply the mandelbrod recursion.
 */
static color_t pixel_color(double complex point, double threshold, uint32_t maxIter,
                           mandelbrot_function_t func)
{
        double complex z = 0;
        color_t color;
        for (color = 0; color < maxIter && cabs(z) < threshold; color++) {
                z = func(z, point);
        }

        return color;
}



/*
 * Compute the color indeces of the mandelbrot set on a certain area of the
 * complex plan.
 */
int compute_window(color_t *image, struct complex_plan_area area, uint32_t width,
                   uint32_t height, double stepX, double stepY, double threshold,
                   uint32_t maxIter, mandelbrot_function_t func)
{
        for (uint32_t x = 0; x < width; x++) {
                double z_real = x * stepX + area.startX;
                for (uint32_t y = 0; y < height; y++) {
                        double z_imag = y * stepY + area.startY;
                        double complex z = z_real + z_imag * I;
                        image[x + width*y] = pixel_color(z, threshold, maxIter, func);
                }
        }

        return 0;
}







/* Save an image as an ASCII file (for portability).
 *  width height
 *  p00 p01 p02 ...
 *  p10 p11 p12 ...
 *
 *  ...
 *
 *  pN0 pN1 pN2 ...
 *
 * NB: the coordinate system in the image has the Y-axis reversed compared to the
 * one on the classical complex plan. So we must invert it.
 */
int save_image(color_t *image, uint32_t width, uint32_t height, FILE *outFile)
{
        int ret;
        ret = fprintf(outFile, "%u %u\n", width, height);
        check(ret >= 0, "Failed to print in file");

        for (int32_t y = height-1; y >= 0; y--) {
                for (uint32_t x = 0; x < width; x++) {
                        fprintf(outFile, "%u ", image[x + width*y]);
                }
                fprintf(outFile, "\n");
        }

        return 0;
}




double complex classic_mandelbrot(double complex z_n, double complex point)
{
        return z_n * z_n + point;
}
