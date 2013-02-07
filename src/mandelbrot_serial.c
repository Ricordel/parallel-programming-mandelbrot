#include <stdlib.h>
#include <stdint.h>
#include <complex.h> /* NOTE: needs a C99 compliant compiler ! */
#include <getopt.h>

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





/**** Command-line options *****/
struct options {
        double threshold;
        uint32_t width;
        uint32_t height;
        uint32_t maxIter;
        uint32_t offsetX;
        uint32_t offsetY;
        double windowX;
        double windowY;
        char *outFileName;
};


// 
// Long command-line option and their short equivalents
//
static const struct option longOpts[] = {
        {"threshold", required_argument, NULL, 't'},
        {"width", required_argument, NULL, 'w'},
        {"height", required_argument, NULL, 'h'},
        {"max-iter", required_argument, NULL, 'm'},
        {"output", required_argument, NULL, 'o'},
        {"x-window", required_argument, NULL, 'x'},
        {"y-window", required_argument, NULL, 'y'},
        {"offset-x", required_argument, NULL, 'a'},
        {"offset-y", required_argument, NULL, 'b'},
};

static const char * shortOpts = "t:w:h:m:o:x:y:a:b:";



int parse_options(struct options *progOptions, int argc, char **argv)
{
        /* Initialize options with default values */
        progOptions->threshold = 4.0;
        progOptions->width = 1600;
        progOptions->height = 900;
        progOptions->maxIter = 30;
        progOptions->outFileName = "out.img";
        progOptions->offsetX = 0;
        progOptions->offsetY = 0;
        progOptions->windowX = 1.0;
        progOptions->windowY = 1.0;

        int ret;
        int opt = getopt_long(argc, argv, shortOpts, longOpts, NULL);
        while (opt != -1) {
                switch (opt) {
                        case 't':
                                ret = sscanf(optarg, "%lf", &progOptions->threshold);
                                check (ret >= 0, "Failed to parse threshold");
                                break;
                        case 'w':
                                ret = sscanf(optarg, "%u", &progOptions->width);
                                check (ret >= 0, "Failed to parse width");
                                break;
                        case 'h':
                                ret = sscanf(optarg, "%u", &progOptions->height);
                                check (ret >= 0, "Failed to parse height");
                                break;
                        case 'm':
                                ret = sscanf(optarg, "%u", &progOptions->maxIter);
                                check (ret >= 0, "Failed to parse maxIter");
                                break;
                        case 'o':
                                progOptions->outFileName = calloc((strlen(optarg) + 1), sizeof(char));
                                check_null(progOptions->maxIter);
                                strcpy(progOptions->outFileName, optarg);
                                break;
                        case 'x': /* x-window */
                                ret = sscanf(optarg, "%lf", &progOptions->windowX);
                                check (ret >= 0, "Failed to parse x-window");
                                break;
                        case 'y': /* y-window */
                                ret = sscanf(optarg, "%lf", &progOptions->windowY);
                                check (ret >= 0, "Failed to parse y-window");
                                break;
                        case 'a': /* x-offset */
                                ret = sscanf(optarg, "%u", &progOptions->offsetX);
                                check (ret >= 0, "Failed to parse x-offset");
                                break;
                        case 'b': /* y-offset */
                                ret = sscanf(optarg, "%u", &progOptions->offsetY);
                                check (ret >= 0, "Failed to parse y-offset");
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

        debug("Command-line args: threshold: %lf, width: %u, height: %u, x-window: %lf, y-window: %lf, "
              "maxIter: %u, x-offset: %u, y-offset: %u, out file: %s\n",
                        progOptions->threshold, progOptions->width, progOptions->height,
                        progOptions->windowX, progOptions->windowY, progOptions->offsetX,
                        progOptions->offsetY, progOptions->maxIter, progOptions->outFileName);

        return 0;
}


int main(int argc, char **argv)
{
        int ret;

        struct options o;
        parse_options(&o, argc, argv);


        /* We will save the result to a file. Try to open it right ahead so
         * we don't compute 5 minutes before knowing we don't haveee the rights
         * on it... */
        FILE * outFile = fopen(o.outFileName, "w");
        check_null(outFile);

        /* Actually compute the image we want */
        color_t **image = allocate_image(o.width, o.height);
        ret = compute_image(image, o.width, o.height, o.threshold, o.maxIter, mandelbrot_function);
        check(ret == 0, "Failed to compute the image");

        /* And save the result to the file */
        ret = save_image(image, o.width, o.height, outFile);
        check(ret == 0, "Failed to save the image to file");

        return 0;
}
