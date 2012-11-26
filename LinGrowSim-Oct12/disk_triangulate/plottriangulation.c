#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 *
Program to generate gnuplot-input from a triangulation file as used
in the Sarande package. Allows to generate illustrations of the
triangulation. Are expecting triangulation of a disk with radius
one, and will give 3-dimensional coordinates of nodes on a hemisphere
with radius one. (This could easily be changed to just generate a plot
of the disk-triangulation in the plane)

Always prints a usage message to stderr.
 *
 */

float Z(float xc, float yc)
{
	float r = xc*xc + yc*yc;

	if (r >= 1) return (0.0);
	return ( sqrt(1 - r) );
}

int main(int argc, char **argv)
{

	int number_of_nodes, number_of_fixed_nodes;
	int *neighbour;
	int *degree;
	int max_degree;
	float *x, *y;
	int i,j;

	fprintf(stderr, "How to use: pipe triangulation file to %s which
will print gnuplot-input of the triangulation to stdout.\n"
		"Pipe this to a file, for example.\n", argv[0]);


	scanf("%d", &number_of_nodes);

	scanf("%d", &number_of_fixed_nodes);

	scanf("%d", &max_degree);

	x = (float *) malloc(number_of_nodes*sizeof(float));
	y = (float *) malloc(number_of_nodes*sizeof(float));
	neighbour = (int *)malloc(number_of_nodes*max_degree*sizeof(int));
	degree = (int *)malloc(number_of_nodes*sizeof(int));

	for (i=0; i<number_of_nodes; i++) {
		scanf("%f", &(x[i]));
		scanf("%f", &(y[i]));
	}
        for (i=0; i<number_of_nodes; i++) {
                int the_neighbour,the_degree;
                scanf( "%d", &the_degree);
                degree[i] = the_degree;
                for (j = 0; j < the_degree; j++) {
                        scanf( "%d", &the_neighbour);
                        neighbour[j+max_degree*i] = the_neighbour;
                }
        }
	for (i=0; i<number_of_nodes; i++) {
		int del;
		for (j=0; j<degree[i]; j++) {
			if (i < neighbour[j+max_degree*i]) {
				int n1, n2;
				n1 = i;
				n2 = neighbour[j+max_degree*i];
				printf("%f %f %f\n",
					x[n1], y[n1], Z(x[n1],y[n1]));
				printf("%f %f %f\n",
					x[n2], y[n2], Z(x[n2],y[n2]));
				printf("\n\n");
			}
		}
	}

	free(x); free(y); free(degree); free(neighbour);
	return(0);
}
