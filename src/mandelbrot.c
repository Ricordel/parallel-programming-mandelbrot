#include <stdlib.h>
#include <stdint.h>
#include <complex.h> /* NOTE: needs a C99 compliant compiler ! */
#include <getopt.h>
#include <math.h>

#include "dbg.h"
#include "mandelbrot.h"



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
static color_t pixel_color(double complex point, double threshold, uint32_t maxIter,
                           mandelbrot_function_t func)
{
        debug("maxIter: %u\n", maxIter);
        double complex z = 0;
        color_t color;
        for (color = 0; color < maxIter && cabs(z) < threshold; color++) {
                z = func(z, point);
        }

        return color;
}



int compute_window(color_t **image, struct complex_plan_area totalArea,
                   double stepX, double stepY, struct sub_image subImage,
                   double threshold, uint32_t maxIter, mandelbrot_function_t func)
{
        for (uint32_t x = subImage.fromX; x < subImage.toX; x++) {
                double z_real = x * stepX + totalArea.startX;
                for (uint32_t y = subImage.fromY; y < subImage.toY; y++) {
                        double z_imag = y * stepY + totalArea.startY;
                        double complex z = z_real + z_imag * I;
                        image[x][y] = pixel_color(z, threshold, maxIter, func);
                }
        }

        return 0;
}







/**
 * Save an image as an ASCII file (for portability). Print the dimensions of
 * the image on the first line, to ease further reading, then the values line by line.
 * NB: the coordinate system in the image has the Y-axis reversed compared to the
 * one on the classical complex plan. So we must invert it.
 */
int save_image(color_t **image, uint32_t width, uint32_t height, FILE *outFile)
{
        int ret;
        ret = fprintf(outFile, "%u %u\n", width, height);
        check(ret >= 0, "Failed to print in file");

        for (int32_t y = height-1; y >= 0; y--) {
                for (uint32_t x = 0; x < width; x++) {
                        ret = fprintf(outFile, "%u ", image[x][y]);
                }
                ret = fprintf(outFile, "\n");
        }

        return 0;
}




double complex mandelbrot_function(double complex z_n, double complex point)
{
        debug("Enter mandelbrot function");
        return z_n * z_n + point;
}







// 
// Long command-line option and their short equivalents
//
static const struct option longOpts[] = {
        {"threshold", required_argument, NULL, 't'},
        {"width", required_argument, NULL, 'w'},
        {"height", required_argument, NULL, 'h'},
        {"max-iter", required_argument, NULL, 'm'},
        {"output", required_argument, NULL, 'o'},
        {"crop", required_argument, NULL, 'c'},
};

static const char * shortOpts = "t:w:h:m:o:c:";



int parse_options(struct prog_options *pProgOptions, int argc, char **argv)
{
        /* Initialize options with default values */
        pProgOptions->threshold = 1.0;
        pProgOptions->width = 1600;
        pProgOptions->height = 900;
        pProgOptions->maxIter = 30;
        pProgOptions->outFileName = "out.img";
        pProgOptions->area.startX = -INFINITY;
        pProgOptions->area.startY = -INFINITY;
        pProgOptions->area.endX = INFINITY;
        pProgOptions->area.endY = INFINITY;

        int ret;
        int opt = getopt_long(argc, argv, shortOpts, longOpts, NULL);
        while (opt != -1) {
                switch (opt) {
                        case 't':
                                ret = sscanf(optarg, "%lf", &pProgOptions->threshold);
                                check (ret >= 0, "Failed to parse threshold");
                                break;
                        case 'w':
                                ret = sscanf(optarg, "%u", &pProgOptions->width);
                                check (ret >= 0, "Failed to parse width");
                                break;
                        case 'h':
                                ret = sscanf(optarg, "%u", &pProgOptions->height);
                                check (ret >= 0, "Failed to parse height");
                                break;
                        case 'm':
                                ret = sscanf(optarg, "%u", &pProgOptions->maxIter);
                                check (ret >= 0, "Failed to parse maxIter");
                                break;
                        case 'o':
                                pProgOptions->outFileName = calloc((strlen(optarg) + 1), sizeof(char));
                                check_null(pProgOptions->maxIter);
                                strcpy(pProgOptions->outFileName, optarg);
                                break;
                        case 'c': /* crop window */
                                ret = sscanf(optarg, "%lf,%lf,%lf,%lf",
                                             &pProgOptions->area.startX, &pProgOptions->area.startY,
                                             &pProgOptions->area.endX, &pProgOptions->area.endY);
                                check (ret >= 0, "Failed to parse crop window");
                                break;
                        case 0: /* Not a short option */
                                log_err("Unknown argument %s\n", argv[optind]);
                                break;
                        default:
                                die("Default case should not be reached !");
                                break;
                }
                opt = getopt_long(argc, argv, shortOpts, longOpts, NULL);
        }

        /* If the crop window was not defined, set it to -thr,-thr,thr,thr */
        if (pProgOptions->area.startX == -INFINITY) {
                pProgOptions->area.startX = -pProgOptions->threshold;
                pProgOptions->area.startY = -pProgOptions->threshold;
                pProgOptions->area.endX   = pProgOptions->threshold;
                pProgOptions->area.endY   = pProgOptions->threshold;
        }

        if (pProgOptions->maxIter > 255) {
                die("The number of iterations cannot be more than 255");
        }

        debug("Command-line args: threshold: %lf, width: %u, height: %u, "
              "maxIter: %u, window: (%lf, %lf, %lf, %lf), out file: %s",
              pProgOptions->threshold, pProgOptions->width, pProgOptions->height,
              pProgOptions->maxIter, pProgOptions->area.startX, pProgOptions->area.startY,
              pProgOptions->area.endX, pProgOptions->area.endY, pProgOptions->outFileName);

        return 0;
}


double complex classic_mandelbrot(double complex z_n, double complex point)
{
        debug("Enter mandelbrot function");
        return z_n * z_n + point;
}

