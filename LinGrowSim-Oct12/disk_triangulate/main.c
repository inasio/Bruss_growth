#include "BM.h"
#include <stdio.h>
#include <stdlib.h>

#define GLOBAL
#include "globals.h"

#include "init_points.h"
#include "place_nodes.h"
#include "neighbours.h"

/*
 *
Code to triangulate a disk.

Method:
Place M many regular polygons of order n (n-gon) inside of each other,
at the same distance, without rotating them.

Example n=4 (square):

	+----------------------+
	|                      |
	|  +----------------+  |
	|  |                |  |
	|  |  +---------+   |  |
	|  |  |         |   |  |
	|  |  |         |   |  |
	|  |  +---------+   |  |
	|  |                |  |
	|  +----------------+  |
	|                      |
	+----------------------+

Place nodes at the corners of the n-gons

	N----------------------N
	|\                   / |
	|  N----------------N  |
	|  | \            / |  |
	|  |  N---------N   |  |
	|  |  |         |   |  |
	|  |  |         |   |  |
	|  |  N---------N   |  |
	|  | /            \ |  |
	|  N----------------N  |
	|/                   \ |
	N----------------------N

Place M nodes (equally spaced)  along each side of the outer n-gon, M-1
nodes along each side of the first-inner n-gon, and so on, placing one
node along side of the most-inner n-gon. Place one node at the center.

Put edges between nodes if they are closest neighbours.

	N-------N-----N--------N
	|\   /   \   /  \    / |
	|  N-------N--------N  |
	|  | \   /    \   / |  |
	|  |  N---------N   |  |
	|  |  | \    /  |   |  |
	|  |  |    N    |   |  |
	|  |  |         |   |  |
	|  |  N---------N   |  |
	|  |                |  |
	|  N----------------N  |
	|                      |
	N----------------------N
(only upper-central sector filled out)

Stretch to obtain a triangulation of the disk with radius one.
(The center has has coordinates (0,0).)

Stretching is done as soon as nodes are placed, not as in the explanation
above, at the end.

Most nodes have 6 neighbours, independently of n, the node in the center has
n many neighbours and the corner-nodes on the boundary have three
neighbours.
 *
 */


/*
 *
Function
	main

Function main calls sub functions in sequence in order to assign
coordinates to nodes and link nodes up to triangles. The coordinates
of the nodes in the plane and the neighbours of each node in clockwise
order are printed to stdout.

The arguments from the command line may be up to two numbers

	N1 [N2]

where the N2 is optional.

The first number specifies the number of nodes along the equator
divided by the second number (M in explanation above). The second
number specifies the order of the center-node, n in explanation above.
It defaults to 6, should none be specified.


Arguments: int and **char (standard command line argument passing)
Returns: nothing meaningful

 *
 */

int main(int argc, char *argv[])
BEGIN
	int i,j;
	int shuffle;


	IF (argc == 1) THEN
		printf("Usage: %s points-on-boundary [center-order]\n", argv[0]);
		return;
	FI



	border_order = atoi(argv[1]);
	IF (argc == 2) THEN
		center_order = 6;
	ELSE
		center_order = atoi(argv[2]);
	FI


	init_points();

        make_first_sequence();

	FOR (i=1; i<border_order-1; i++) DO
		add_sequence(i);
	OD

	add_last_element();

	give_first_node_sequence_neighbours();

	FOR(i=1; i<border_order-2; i++) DO
		give_node_sequence_neighbours(i);
	OD


	give_last_nodes_neighbours();

	fprintf(stderr, "Have %d points. Writing ..\n",
		next_point);
	fflush(stderr);

	printf("%d\n",next_point);

	/* Number of nodes on the boundary. */
	printf("%d\n", center_order*(border_order - 1));

#define MAX(n1,n2) ((n1) > (n2) ? (n1) : (n2))

	printf("%d\n", MAX(6, center_order));

	FOR (i=0; i<next_point; i++) DO
		IF ( (i % 361) == 1)
		fprintf(stderr, "\rWriting node %d", i);
		printf("%2.15f  %2.15f\n", x[i],y[i]);
	OD


	FOR (i=0; i<next_point; i++) DO
		int dd = 0;

		IF ( (i % 361) == 1) THEN
			fprintf(stderr, "\rWriting node neighbours for node %d            ", i);
		FI
		FOR (j=0; j<MAX(center_order, 6); j++) DO
			IF (node_nl(i,j) >= 0) THEN dd++; FI
		OD
		printf("%d ", dd);
		FOR (j=0; j<dd; j++) DO
			printf("%d ", node_nl(i,j));
		OD
		printf("\n");
	OD

	free(x); free(y); free(t_array); free(p); free(p_inverse);
	free(node_nl_array); free(node_tl_array);
	free(triangle_nl_array);
	fprintf(stderr, "\rDone.                                                              \n");
	return(0);
END


		
