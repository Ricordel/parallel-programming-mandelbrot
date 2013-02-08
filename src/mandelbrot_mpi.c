#include <stdlib.h>
#include <mpi.h>

#include "dbg.h"
#include "option_parser.h"
#include "mandelbrot.h"




/**
 * Determining the number of the first line handeled by a certain node
 */
static inline int lines_at_node(int nodeNumber, int nbNodes, uint32_t nbLines)
{
        int d = nbLines / nbNodes;
        int m = nbLines % nbNodes;

        if (nodeNumber < m) {
                return d + 1;
        } else {
                return d;
        }
}


static inline int first_line_of_node(int nodeNumber, int nbNodes, uint32_t nbLines)
{
        int d = nbLines / nbNodes;
        int m = nbLines % nbNodes;

        if (nodeNumber == 0) {
                return 0; /* Does not fit in the following formula */
        } else if (nodeNumber < m) {
                return (d + 1) * nodeNumber - 1;
        } else {
                return (d + 1) * m + d * (nodeNumber - m) - 1;
        }
}





int main(int argc, char **argv)
{
        int ret;

        ret = MPI_Init(&argc, &argv);
        check(ret == MPI_SUCCESS, "Failed to init MPI");

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

        /* In a first time, check the number of lines is multiple of the number
         * of nodes. */
        int nbNodes, rank;
        ret = MPI_Comm_size(MPI_COMM_WORLD, &nbNodes);
        check (ret == MPI_SUCCESS, "Failed to get comm size");

        ret = MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        check (ret == MPI_SUCCESS, "Failed to get comm rank");



        /****** Now starts the differentiation between the master node and others *****/

        debug("Node %d: handles %d lines from %d\n", rank,
              lines_at_node(rank, nbNodes, o.height), first_line_of_node(rank, nbNodes, o.height));
        
        /* Only the master node allocates the whole image, others allocate smaller ones */
        color_t *image = NULL;
        if (rank == 0) {
                image = allocate_image(o.width, o.height);
        } else {
                image = allocate_image(o.width, lines_at_node(rank, nbNodes, o.height));
        }

        /* Everybody computes his part of the image */
        struct complex_plan_area myArea = {
                o.area.startX, /* From the left... */
                o.area.startY + first_line_of_node(rank, nbNodes, o.height) * stepY, /* start of my slice */
                o.area.endX, /* ... to the right */
                o.area.startY + (first_line_of_node(rank + 1, nbNodes, o.height) - 1) * stepY,
        };

        ret = compute_window(image, myArea, o.width, lines_at_node(rank, nbNodes, o.height),
                             stepX, stepY, o.threshold, o.maxIter, classic_mandelbrot);
        check(ret == 0, "Failed to compute the image");


        /* Now, the master waits for other to send their results */
        MPI_Status status;
        if (rank == 0) {
                for (int node = 1; node < nbNodes; node++) {
                        debug("Receiving from node %d...", node);
                        ret = MPI_Recv(&image[first_line_of_node(node, nbNodes, o.height) * o.width],
                                       o.width * lines_at_node(node, nbNodes, o.height),
                                       MPI_BYTE, node, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                        check(ret == MPI_SUCCESS, "Node 0: Failed to receive from node %d\n", node);
                }

                /* And save the result to the file */
                ret = save_image(image, o.width, o.height, outFile);
                check(ret == 0, "Failed to save the image to file");
        } else {
                ret = MPI_Send(image, o.width * lines_at_node(rank, nbNodes, o.height), MPI_BYTE, 0, 1, MPI_COMM_WORLD);
                check(ret == MPI_SUCCESS, "Node %d: failed to send to master", rank);
        }


        ret = MPI_Finalize();
        check(ret == MPI_SUCCESS, "Failed to finalize MPI");

        return 0;
}
