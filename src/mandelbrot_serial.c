#include <stdlib.h>
#include <stdint.h>
#include <complex.h> /* NOTE: needs a C99 compliant compiler ! */

#include "dbg.h"



/**
 * A color is simply a number (255 colors will be enough)
 */
typedef uint8_t color_t;



/**
 * Allocate a 2-dimensional array of colors.
 * We want to index our image [x][y] with x on the horizontal direction,
 * so x must be able to go from 0 to nCols - 1, and y from 0 to nRows - 1
 */
color_t ** allocate_image(uint32_t nCols, uint32_t nRows)
{
        color_t ** image = (color_t **) calloc(nCols, sizeof(color_t *));
        check_mem(image);

        for (uint32_t i = 0; i < nCols; i++) {
                image[i] = (color_t *) calloc(nRows, sizeof(color_t));
                check_mem(image[i]);
        }

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
color_t pixel_color(double complex point, double threshold, uint32_t maxIter,
                    mandelbrot_function_t func)
{
        double complex z = 0;
        color_t color;
        for (color = 0; color < maxIter && cabs(z) < threshold; color++) {
                z = func(z, point);
        }

        return color;
}




int compute_image(color_t **image, uint32_t width, uint32_t height,
                   double threshold, uint32_t maxIter, mandelbrot_function_t func)
{
        double stepX = (2 * threshold) / (double)(width - 1);
        double stepY = (2 * threshold) / (double)(height - 1);

        for (uint32_t x = 0; x < width; x++) {
                double z_real = x * stepX - threshold;
                for (uint32_t y = 0; y < height; y++) {
                        double z_imag = y * stepY - threshold;
                        double complex z = z_real + z_imag * I;
                        image[x][y] = pixel_color(z, threshold, maxIter, func);
                }
        }

        return 0;
}



/**
 * Save an image as an ASCII file (for portability). Print the dimensions of
 * the image on the first line, to ease further reading, then the values line by line.
 */
int save_image(color_t **image, uint32_t width, uint32_t height, FILE *outFile)
{
        int ret;
        ret = fprintf(outFile, "%u %u\n", width, height);
        check(ret >= 0, "Failed to print in file");

        for (uint32_t y = 0; y < height; y++) {
                for (uint32_t x = 0; x < width; x++) {
                        ret = fprintf(outFile, "%u ", image[x][y]);
                }
                ret = fprintf(outFile, "\n");
        }

        return 0;
}




double complex mandelbrot_function(double complex z_n, double complex point)
{
        return z_n * z_n + point;
}


int main(void)
{
        int ret;

        //NOTE: tout ceci sera à prendre sur la ligne de commande !
        double threshold = 4.0; /* Why not ? */
        uint32_t width = 1600;
        uint32_t height = 900;
        uint32_t maxIter = 30;

        /* We will save the result to a file. Try to open it right ahead so
         * we don't compute 5 minutes before knowing we don't haveee the rights
         * on it... */
        FILE * outFile = fopen("./out.img", "w");
        check_null(outFile);

        /* Actually compute the image we want */
        color_t **image = allocate_image(width, height);
        ret = compute_image(image, width, height, threshold, maxIter, mandelbrot_function);
        check(ret == 0, "Failed to compute the image");

        /* And save the result to the file */
        ret = save_image(image, width, height, outFile);
        check(ret == 0, "Failed to save the image to file");

        return 0;
}
