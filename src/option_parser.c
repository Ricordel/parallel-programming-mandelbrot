#include <getopt.h>
#include <string.h>
#include <math.h>

#include "dbg.h"
#include "option_parser.h"



/* 
 * Long command-line option and their short equivalents
 */
static const struct option longOpts[] = {
        {"threshold", required_argument, NULL, 't'},
        {"width", required_argument, NULL, 'w'},
        {"height", required_argument, NULL, 'h'},
        {"max-iter", required_argument, NULL, 'm'},
        {"output", required_argument, NULL, 'o'},
        {"crop", required_argument, NULL, 'c'},
        {"help", no_argument, NULL, 'H'},
        {NULL, 0, 0, 0} /* To prevent crash in case of illegal option */
};

static const char * shortOpts = "Ht:w:h:m:o:c:";





static void print_help()
{
        puts("Usage: prog-name [args]\n");

        puts("Available arguments:");
        puts("\t-H --help              Print this message and exit");
        puts("\t-t --threshold thr     Threshold after which we consider a point is divergent");
        puts("\t-w --width w           Width, in pixels, of the output image");
        puts("\t-h --height h          Height, in pixels, of the output image");
        puts("\t-m --max-iter m        Maximum number of iterations before considering the point is stationnary");
        puts("\t-c --crop x0,y0,x1,y1  Section of the complex plan that will be considered\n"
             "\t                       from (x0, y0) (bottom-left) to (x1,y1) (top-right)");
        puts("\t-o --output path       Name of the output file");
}






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
                        case 'H':
                                print_help();
                                exit(0);
                                break;
                        case 0: /* Not a short option */
                                log_err("Unknown argument %s\n", argv[optind]);
                                exit(1);
                                break;
                        default:
                                log_err("Unknown argument %s\n", argv[optind]);
                                exit(1);
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
