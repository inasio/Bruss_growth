#include "BM.h"
#include <stdio.h>
#include <stdlib.h>

#define GLOBAL extern
#include "globals.h"

#include <math.h>

/*
 *
Function
	first_node_number_of_sequence

Determines the index of the first node of the sequence specified.
 *
 */
int first_node_number_of_sequence(int s)
BEGIN
	return ( s*center_order*border_order - s*(s+1)/2*center_order);
END


/*
 *
Function
	give_node_node_neighbours

Function give_node_node_neighbours sets the neighbours for the node
given as argument one. The neighbours are assigned in the order given
(n1..n6) by the remaining arguments.

Arguments: 7 ints.
Returns: void
 *
 */
void give_node_node_neighbours(int n, int n1, int n2, int n3, int n4, int n5, int n6)
BEGIN
	node_nl(n, 0) = n1;
	node_nl(n, 1) = n2;
	node_nl(n, 2) = n3;
	node_nl(n, 3) = n4;
	node_nl(n, 4) = n5;
	node_nl(n, 5) = n6;
END

/*
 *
Function
	give_first_node_sequence_neighbours

Function give_first_node_sequence_neighbours assignes neighbours to the
ndoes along the boundary. Degree of the center node many  of these have only
degree three. This is not so for the other sequences. This is why there is
a special function for this case.

Arguments: none
Returns: void
 *
 */

void give_first_node_sequence_neighbours()
BEGIN
	int i,j;
	int node, inner_node;

	give_node_node_neighbours(0,first_node_number_of_sequence(1)-1,
				    first_node_number_of_sequence(1),
				    1,
				    -1,-1,-1);

	node = 1; inner_node = first_node_number_of_sequence(1);

	FOR (i=0; i<center_order-1; i++) DO
		FOR(j=1; j<border_order-1; j++) DO
			node_nl(node, 0) = node-1;
			node_nl(node, 1) = inner_node;
			node_nl(node, 2) = inner_node+1;
			node_nl(node, 3) = node+1;

			node++; inner_node++;
		OD
		node_nl(node, 0) = node-1;
		node_nl(node, 1) = inner_node;
		node_nl(node, 2) = node+1;

		node++;
	OD

	FOR(j=1; j<border_order-2; j++) DO
		node_nl(node, 0) = node-1;
		node_nl(node, 1) = inner_node;
		node_nl(node, 2) = inner_node+1;
		node_nl(node, 3) = node+1;

		node++; inner_node++;
	OD

	node_nl(node, 0) = node -1;
	node_nl(node, 1) = inner_node;
	node_nl(node, 2) = node +1;
	node_nl(node, 3) = 0;
END

/*
 *
Function
	give_node_sequence_neighbours

Function give_node_sequence_neighbours assigns neighbours to all nodes that
do not lie on the boundary and are at least one sequence away from the center.
The number of the sequence is specified as the argument.

Arguments: one int
Returns: void
 *
 */

void give_node_sequence_neighbours(int s)
BEGIN
	int i,j;
	int node, inner_node;

	int first_node;
	int node_previous_sequence, node_next_sequence;

	first_node = first_node_number_of_sequence(s);
	node_nl(first_node, 0) = first_node_number_of_sequence(s-1);
	node_nl(first_node, 1) = first_node - 1;
	node_nl(first_node, 2) = first_node_number_of_sequence(s+1) - 1;
	node_nl(first_node, 3) = first_node_number_of_sequence(s+1);
	node_nl(first_node, 4) = first_node + 1;
	node_nl(first_node, 5) = first_node_number_of_sequence(s-1) + 1;

	node = first_node + 1;
	node_previous_sequence = first_node_number_of_sequence(s-1) + 1;
	node_next_sequence = first_node_number_of_sequence(s+1);


	FOR (i=0; i<center_order-1; i++) DO
		FOR(j=1; j<border_order-s-1; j++) DO
			node_nl(node, 0) = node_previous_sequence;
			node_nl(node, 1) = node - 1;
			node_nl(node, 2) = node_next_sequence;
			node_nl(node, 3) = node_next_sequence+1;
			node_nl(node, 4) = node + 1;
			node_nl(node, 5) = node_previous_sequence + 1;

			node++; node_previous_sequence++; node_next_sequence++;
		OD
		node_nl(node, 0) = node_previous_sequence;
		node_nl(node, 1) = node - 1;
		node_nl(node, 2) = node_next_sequence;
		node_nl(node, 3) = node + 1;
		node_nl(node, 4) = node_previous_sequence + 2;
		node_nl(node, 5) = node_previous_sequence + 1;

		node++; node_previous_sequence++; node_previous_sequence++;
	OD
	FOR(j=1; j<border_order-s-2; j++) DO
		node_nl(node, 0) = node_previous_sequence;
		node_nl(node, 1) = node - 1;
		node_nl(node, 2) = node_next_sequence;
		node_nl(node, 3) = node_next_sequence+1;
		node_nl(node, 4) = node + 1;
		node_nl(node, 5) = node_previous_sequence + 1;

		node++; node_previous_sequence++; node_next_sequence++;
	OD
	node_nl(node, 0) = node_previous_sequence;
	node_nl(node, 1) = node - 1;
	node_nl(node, 2) = node_next_sequence;
	node_nl(node, 3) = node + 1;
	node_nl(node, 4) = node_previous_sequence + 2;
	node_nl(node, 5) = node_previous_sequence + 1;

	node++; node_previous_sequence++; node_previous_sequence++;


END

/*
 *
Function
	give_last_nodes_neighbours

Function give_last_nodes_neighbours assigns neighbours to the center
node and its neighbours. The difference to the other nodes is that all
neighbours of the center are connected to the same node. This is not so
for nodes in the other sequences.

Arguments: none
Returns: void
 *
 */


void give_last_nodes_neighbours()
BEGIN
	int i;
	int node, node_previous_sequence;

	node = first_node_number_of_sequence(border_order - 2);
	node_previous_sequence = first_node_number_of_sequence(border_order - 3);

	node_nl(node, 0) = node_previous_sequence;
	node_nl(node, 1) = node - 1;
	node_nl(node, 2) = node + center_order-1;
	node_nl(node, 3) = node + center_order;
	node_nl(node, 4) = node + 1;
	node_nl(node, 5) = node_previous_sequence+1;

	node_previous_sequence = node_previous_sequence + 2;

	node ++;
	FOR (i=1; i<center_order  -1; i++) DO
		node_nl(node, 0) = node_previous_sequence;
		node_nl(node, 1) = node_previous_sequence-1;
		node_nl(node, 2) = node - 1;
		node_nl(node, 3) = first_node_number_of_sequence(border_order - 1);
		node_nl(node, 4) = node + 1;
		node_nl(node, 5) = node_previous_sequence + 1;

		node_previous_sequence += 2;

		node ++;
	OD

	node_nl(node, 0) = node_previous_sequence;
	node_nl(node, 1) = node_previous_sequence - 1;
	node_nl(node, 2) = node - 1;
	node_nl(node, 3) = first_node_number_of_sequence(border_order - 1);
	node_nl(node, 4) = first_node_number_of_sequence(border_order - 2);
	node_nl(node, 5) = node_previous_sequence + 1;


	node = first_node_number_of_sequence(border_order - 1);

	node_previous_sequence = first_node_number_of_sequence(border_order - 2);

	node_nl(node, 0) = node_previous_sequence;

	FOR (i=1; i<center_order; i++) DO
		node_nl(node, i) = node_previous_sequence + center_order-i;
	OD


END
