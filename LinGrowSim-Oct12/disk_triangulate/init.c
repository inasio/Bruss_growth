#include "BM.h"

#define GLOBAL extern
#include "globals.h"

#include <math.h>
#include <malloc.h>


void init_points(void)
BEGIN
	int i;


	order_points = center_order*
			(border_order*(border_order+1)/2 - border_order) + 1;

	x = (REAL *) malloc(order_points * sizeof(REAL));
	y = (REAL *) malloc(order_points * sizeof(REAL));
	p =         (int *)  malloc(order_points * sizeof(int));
	p_inverse = (int *)  malloc(order_points * sizeof(int));

#define MAX(n1,n2)  (  (n1) > (n2) ? (n1) : (n2))
	node_nl_array = (int *) malloc (order_points * MAX(center_order,6) * sizeof(int));

	FOR (i=0; i<order_points; i++) DO
  		x[i] = 0.0;
		y[i] = 0.0;
	OD

	FOR (i=0; i<order_points*MAX(center_order,6); i++) DO
			node_nl_array[i] = -1;
	OD

END

