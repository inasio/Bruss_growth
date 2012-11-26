#include "BM.h"
#include "types.h"
#include "globals.h"
#include <stdio.h>
#include <math.h>
//#define GLOBAL extern
//#include "memory.h"


#ifdef USE_TRIANGLES
REAL X_of_triangle(triangle *t)
BEGIN
	return ( ( t->n[0]->X + t->n[1]->X + t->n[2]->X ) / 3.0);
END

REAL Y_of_triangle(triangle *t)
BEGIN
	return ( ( t->n[0]->Y + t->n[1]->Y + t->n[2]->Y ) / 3.0);
END

	
REAL triangle_area (triangle *t)
BEGIN
	REAL x1,x2,x3,y1,y2,y3,z1,z2,z3;
       	REAL area, subterm;

	x1 = t->n[0]->x; y1 = t->n[0]->y; z1 = t->n[0]->z;
	x2 = t->n[1]->x; y2 = t->n[1]->y; z2 = t->n[1]->z;
	x3 = t->n[2]->x; y3 = t->n[2]->y; z3 = t->n[2]->z;

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
#endif
	
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

#ifdef USE_TRIANGLES
void set_normal_vector(triangle *t)
BEGIN
	REAL 	vx, vy, vz,  
		ax,ay,az,  
		bx,by,bz,  
	        x0,x1,x2, 
		y0,y1,y2, 
		z0,z1,z2,
		length;

	x0 = t->n[0]->x; y0 = t->n[0]->y; z0 = t->n[0]->z;
	x1 = t->n[1]->x; y1 = t->n[1]->y; z1 = t->n[1]->z;
	x2 = t->n[2]->x; y2 = t->n[2]->y; z2 = t->n[2]->z;

	ax = x1 - x0;
	ay = y1 - y0;
	az = z1 - z0;

	bx = x2 - x0;
	by = y2 - y0;
	bz = z2 - z0;

	vx = az*by - ay*bz;
	vy = ax*bz - az*bx;
	vz = ay*bx - ax*by;

	length = sqrt( vx*vx + vy*vy + vz*vz );

	t->normal_x = vx / length;
	t->normal_y = vy / length;
	t->normal_z = vz / length;
END

	
void give_triangle(triangle *t)
BEGIN
	printf("Triangle is %d with nodes %d %d %d.\n",
		t->id, t->n[0]->id, t->n[1]->id, t->n[2]->id);
END	



void find_average_X_value(void)
BEGIN
	triangle_list *tl;


	REAL total_area =0.0;
	REAL total_X    =0.0;

	tl = first_tl;
	WHILE(tl!= NULL) DO
		triangle *t = tl->entry;

		total_X += X_of_triangle(t) * t->area;

		total_area += t->area;


		tl = tl->next;
	OD
 	printf("Brusselation[%d]:Area = %f Tot-X = %f Av-X = %f.\n",
		pid,total_area, total_X, total_X/total_area);

END
#endif

void count_node_degrees_tr()
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


        printf("Brusselation[%d]: Maximal degree is %d.\n",
                pid, max_degree);
        printf("Brusselation[%d]: Number of Nodes: %d.\n",
                pid,node_counter);

END
