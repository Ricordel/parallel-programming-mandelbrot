#ifndef __OPTION_PARSER_H__
#define __OPTION_PARSER_H__

#include <stdint.h>

#include "mandelbrot.h"


/**** Command-line options *****/
struct prog_options {
        double threshold;
        uint32_t width;
        uint32_t height;
        uint32_t maxIter;

        struct complex_plan_area area;

        char *outFileName;
};



/* Parses the command-line */
int parse_options(struct prog_options *pProgOptions, int argc, char **argv);



#endif /* end of include guard: __OPTION_PARSER_H__ */
