#include <stdlib.h>

#include "dbg.h"
#include "option_parser.h"
#include "mandelbrot.h"




int main(int argc, char **argv)
{
        int ret;

        struct prog_options o;
        parse_options(&o, argc, argv);


        /* We will save the result to a file. Try to open it right ahead so
         * we don't compute 5 minutes before knowing we don't haveee the rights
         * on it... */
        FILE * outFile = fopen(o.outFileName, "w");
        check_null(outFile);

        /* Compute the steps */
        double stepX = (o.area.endX - o.area.startX) / (double)(o.width - 1);
        double stepY = (o.area.endY - o.area.startY) / (double)(o.height - 1);

        /* Actually compute the image we want */
        color_t *image = allocate_image(o.width, o.height);
        ret = compute_window(image, o.area, o.width, o.height, stepX, stepY,
                             o.threshold, o.maxIter, classic_mandelbrot);
        check(ret == 0, "Failed to compute the image");


        /* And save the result to the file */
        ret = save_image(image, o.width, o.height, outFile);
        check(ret == 0, "Failed to save the image to file");

        return 0;
}
