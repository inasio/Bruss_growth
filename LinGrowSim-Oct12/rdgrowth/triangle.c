#include "BM.h"
#include <stdio.h>
#include <math.h>
#include "types.h"
#define GLOBAL extern
#include "globals.h"
#include "memory.h"



REAL area_of_triangle (node *n0, node*n1, node*n2)
BEGIN
	REAL x1,x2,x3,y1,y2,y3,z1,z2,z3;
       	REAL area, subterm;

	x1 = n0->x; y1 = n0->y; z1 = n0->z;
	x2 = n1->x; y2 = n1->y; z2 = n1->z;
	x3 = n2->x; y3 = n2->y; z3 = n2->z;

	subterm = z3*y2 - z3*y1 - z1*y2 - y3*z2 + y3*z1 + y1*z2;
	subterm = subterm * subterm;
	area = subterm;
	subterm = x3*z2 - x3*z1 - x1*z2 - z3*x2 + z3*x1 + z1*x2;
	subterm = subterm * subterm;
	area += subterm;
	subterm = y3*x2 - y3*x1 - y1*x2 - x3*y2 + x3*y1 + x1*y2;
	subterm = subterm * subterm;
	area += subterm;

/* The  area is given by half of the square root of the number
   held in area at this point. *************************************/

	area = 0.5 * sqrt(area);

	return(area);
END

void count_node_degrees()
BEGIN
        int max_degree = 0;
        int node_counter = 0;
        node_list *nl;
        node *n;


        nl = first_nl;
        WHILE (nl != NULL) DO
	        int dc;
		node_list *nbl;
                node_counter ++;

                n = nl -> entry;
                nbl = n -> nl;
                dc = 0;
                WHILE (nbl != NULL) DO
                        dc += 1;
                        nbl = nbl -> next;
                OD
                IF (dc > max_degree) THEN max_degree = dc; FI

                nl = nl -> next;
        OD

	number_of_nodes = node_counter;


        fprintf(stderr, "SRD_Growth[%d]: Maximal degree is %d.\n",
                pid, max_degree);
        fprintf(stderr, "SRD_Growth[%d]: Number of Nodes: %d.\n",
                pid,node_counter);

END
