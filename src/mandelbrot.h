#ifndef __MANDELBROT_H__
#define __MANDELBROT_H__



#include <stdint.h>
#include <complex.h> /* NOTE: needs a C99 compliant compiler ! */




/**
 * A color is simply a number (255 colors will be enough)
 */
typedef uint8_t color_t;



/**
 * Area of the complex plan to be mapped on the final image
 */
struct complex_plan_area {
        double startX;
        double startY;
        double endX;
        double endY;
};



/**
 * Type for an iteration function, applied to the current value
 * of the sequence and to the point
 */
typedef double complex (mandelbrot_function_t(double complex z_n, double complex point));





/**
 * Allocate a 1-dimensional array of colors big enough to contain an
 * image of nCols columns and nRows rows.
 *
 * in: nCols     Number of columns of the image
 * in: nRows     Number of rows of the image
 *
 * Returns      An array that can contain the image.
 * Failures     Aborts the program if there's no more memory
 */
color_t *allocate_image(uint32_t nCols, uint32_t nRows);



/**
 * Compute the color indeces of the mandelbrot set on a certain area of the
 * complex plan.
 *
 * inout: image  The image to output
 *
 * in: area      The area (in the complex plan) to handle
 * in: width     Width of the image
 * in: height    Height of the image
 * in: stepX     Difference on the x-axis between two consecutive pixels
 *               in the complex plan.
 * in: stepY     Difference on the y-axis between two consecutive pixels
 *               in the complex plan.
 * in: threashold    Limit after which we consider the point is not quasi-stationnary
 * in: maxIter   Maximum number of iterations to make
 * in: func      Function to use in the recursive definition of the Mandelbrot sequance
 *
 * Returns: 0, always successful
 * Failures: None
 */
int compute_window(color_t *image, struct complex_plan_area area, uint32_t width,
                   uint32_t height, double stepX, double stepY, double threshold,
                   uint32_t maxIter, mandelbrot_function_t func);




/**
 * Save an image as an ASCII file (for portability).
 * The format of the file is:
 *
 *  width height
 *  p00 p01 p02 ...
 *  p10 p11 p12 ...
 *  ...
 *  pN0 pN1 ...
 *
 * in: image    The image to write
 * in: width    Width of the image
 * in: height   Height of the image
 * in: outFile  Descriptor to the file in which the image should be written
 *
 * Returns: 0, always suscessful
 * Failures: None
 */
int save_image(color_t *image, uint32_t width, uint32_t height, FILE *outFile);





/****************** Some functions used to define Mandelbrot sequences **********************/

/**
 * "Classical" Mandelbrot function, f(z_n) = z_n^2 + point
 */
double complex classic_mandelbrot(double complex z_n, double complex point);

double complex power_3(double complex z_n, double complex point);

double complex power_4(double complex z_n, double complex point);

double complex power_5(double complex z_n, double complex point);

double complex power_6(double complex z_n, double complex point);

double complex mandelbrot_sin(double complex z_n, double complex point);

double complex mandelbrot_cos(double complex z_n, double complex point);

double complex mandelbrot_log(double complex z_n, double complex point);

double complex tricorn(double complex z_n, double complex point);

#endif /* end of include guard: __MANDELBROT_H__ */
