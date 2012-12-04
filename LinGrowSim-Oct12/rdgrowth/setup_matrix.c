#include "BM.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#define GLOBAL extern
#include "globals.h"
#include "memory.h"
#include "triangle.h"
#include "display.h"

struct vector {
	double entry_x;
	double entry_y;
	double entry_z;
};

void set_the_normal_vector (struct vector *v, node *n0, node *n1, node *n2)
BEGIN
	double 	ax,ay,az,
		bx,by,bz,
	        x0,x1,x2,
		y0,y1,y2,
		z0,z1,z2;

	x0 = n0->x; y0 = n0->y; z0 = n0->z;
	x1 = n1->x; y1 = n1->y; z1 = n1->z;
	x2 = n2->x; y2 = n2->y; z2 = n2->z;

	ax = x1 - x0;
	ay = y1 - y0;
	az = z1 - z0;

	bx = x2 - x0;
	by = y2 - y0;
	bz = z2 - z0;

	v->entry_x = az*by - ay*bz;
	v->entry_y = ax*bz - az*bx;
	v->entry_z = ay*bx - ax*by;
END


void change_to_local_coordinate_system(node *n)
BEGIN

	node_list *nl;
	node *n1;
	struct vector normal;
	double N[3], X[3], Y[3];
	double determinant;
	double length;
	short I;
	/*  find the average of the normal vectors of the triangles the */
	/*  node is incident on */
	N[0]=0.0;
	N[1]=0.0;
	N[2]=0.0;

	nl = n->nl;
	n1 = nl->entry;
	WHILE (nl->next != NULL) DO
		node *nb1 = nl->entry, *nb2 = nl->next->entry;
		set_the_normal_vector(&normal, n,nb1,nb2);
		N[0] += normal.entry_x;
		N[1] += normal.entry_y;
		N[2] += normal.entry_z;
		nl=nl->next;
	OD

	IF (! n->fixed) THEN
		set_the_normal_vector(&normal, n, nl->entry, n1);
		N[0] += normal.entry_x;
		N[1] += normal.entry_y;
		N[2] += normal.entry_z;
	FI

	length = sqrt( N[0] * N[0] + N[1] * N[1] + N[2] * N[2] );

	N[0] = N[0] / length;
	N[1] = N[1] / length;
	N[2] = N[2] / length;

	length = sqrt( N[0] * N[0] + N[1] * N[1] + N[2] * N[2] );


	I = 0;
	IF (ABS(N[1]) > ABS(N[0])) THEN I = 1; FI
	IF (ABS(N[2]) > ABS(N[I])) THEN I = 2; FI


	/* Now I is such that N[I] >= N[0], N[1], N[2] */

	switch(I)
	BEGIN
	     case 0:
		X[1] = n->nl->entry->y - n->y;
		X[2] = n->nl->entry->z - n->z;
		X[0] = - (N[1]*X[1] + N[2]*X[2]) / N[0]; break;
	     case 1:
		X[0] = n->nl->entry->x - n->x;
		X[2] = n->nl->entry->z - n->z;
		X[1] = - (N[0]*X[0] + N[2]*X[2]) / N[1]; break;
	     case 2:
		X[0] = n->nl->entry->x - n->x;
		X[1] = n->nl->entry->y - n->y;
		X[2] = - (N[0]*X[0] + N[1]*X[1]) / N[2]; break;
	     default:
		fprintf(stderr, "Switch-error in change_to_local_coordinate_system!\n");
		EXIT;

	END

	/*    if((X[0]*N[0]+X[1]*N[1]+X[2]*N[2])<0)
    {
       printf("X dot N is %f,%f,%f,%f\n,%f,%f,%f\n",X[0]*N[0]+X[1]*N[1]+X[2]*N[2],X[0],X[1],X[2],N[0],N[1],N[2]);
       } */


	length = sqrt ( X[0]*X[0] + X[1]*X[1] + X[2]*X[2] );

/*        if(length!=1.0)
        {
          printf("SMALL X LENGTH, %.15f,%.15f,%.15f,%15f\n,%.15f,%.15f,%.15f,%d\n,%.15f,%.15f,%.15f,%.15f\n,%.15f,%.15f\n",length,X[0],X[1],X[2],N[0],N[1],N[2],I,n->nl->entry->y,n->y,n->nl->entry->z,n->z,n->nl->entry->x,n->x);
	  } */


	X[0] = X[0] / length;
	X[1] = X[1] / length;
	X[2] = X[2] / length;

	length = sqrt ( X[0]*X[0] + X[1]*X[1] + X[2]*X[2] );



	Y[0] = N[1]*X[2] - N[2]*X[1];
	Y[1] = N[2]*X[0] - N[0]*X[2];
	Y[2] = N[0]*X[1] - N[1]*X[0];

	length = sqrt ( Y[0]*Y[0] + Y[1]*Y[1] + Y[2]*Y[2] );

	Y[0] = Y[0] / length;
	Y[1] = Y[1] / length;
	Y[2] = Y[2] / length;

	length = sqrt ( Y[0]*Y[0] + Y[1]*Y[1] + Y[2]*Y[2] );

/*        if(length!=1.0)
        {
	printf("Y not normalized %.15f,%.15f,%.15f,%.15f\n",length,Y[0],Y[1],Y[2]);
	}   */


/* length of Y should be 1! */

/*	IF (ABS( 1.0 - sqrt ( X[0]*X[0] + X[1]*X[1] + X[2]*X[2] )) > 0.5) THEN
     printf("\n X vector with length %f,%f,%f\n",X[0],X[1],X[2]);
		EXIT;
		FI */


	/* Now have vectors X and Y such that X,Y,N form an orthonormal
	  right-handed system so that the determinant (X Y N) is one. */

	nl = n->nl;
	WHILE(nl != NULL) DO
		node *nb = nl->entry;
		nb->local_x = ((nb->x - n->x) * (Y[1]*N[2] - N[1]*Y[2]) -
			       (nb->y - n->y) * (Y[0]*N[2] - N[0]*Y[2]) +
			       (nb->z - n->z) * (Y[0]*N[1] - N[0]*Y[1]));

		nb->local_y =-((nb->x - n->x) * (X[1]*N[2] - N[1]*X[2]) -
			       (nb->y - n->y) * (X[0]*N[2] - N[0]*X[2]) +
			       (nb->z - n->z) * (X[0]*N[1] - N[0]*X[1]));

		nl = nl->next;
	OD

END

void add_to_coefficients(node *n, node *n1, node *n2,
		         double *Lapl_c1, double *Lapl_c2,
			 double *Area_c1, double *Area_c2)
BEGIN
	double y1,y2,x1,x2, delta_y, delta_x;
	double triangle_area;
	double area_factor;

	triangle_area = area_of_triangle(n,n1,n2);
	area_factor  = 0.25 / triangle_area;


	x1 = n1->local_x;
	x2 = n2->local_x;

	y1 = n1->local_y;
	y2 = n2->local_y;

	delta_x = x1 - x2;
	delta_y = y1 - y2;


	/* Laplacian term : */

	*Lapl_c1      -= area_factor * ( delta_y * y2    + delta_x * x2);
	*Lapl_c2      += area_factor * ( delta_y * y1    + delta_x * x1);
	n->Lapl_coeff -= area_factor * ( delta_y*delta_y + delta_x*delta_x);

	/* non-linear/linear terms */
	*Area_c1      += triangle_area / 12.0;
	*Area_c2      += triangle_area / 12.0;
	n->Area_coeff += triangle_area / 6.0;



END

/* following called after growth and after refine */

void set_coefficients_for_node(node *n)
BEGIN
	coeff_list *cl, *Lapl_cl, *Area_cl;
	node_list *nl;

	node *n1, *n2, *n0;
	double *Lapl_c0, *Lapl_c1, *Lapl_c2,
	     *Area_c0, *Area_c1, *Area_c2;

       	change_to_local_coordinate_system(n);


	/* set coefficients to zero */
	n->Lapl_coeff = 0.0;
	n->Area_coeff = 0.0;

	cl = n->Lapl_coeff_l;
	WHILE(cl != NULL) DO
		cl->entry = 0.0;
		cl = cl->next;
	OD
	cl = n->Area_coeff_l;
	WHILE(cl != NULL) DO
		cl->entry = 0.0;
		cl = cl->next;
	OD

	/* determine coefficients */
	nl = n->nl;
	n1 = nl->entry;
	nl = nl->next;
	Lapl_cl = n->Lapl_coeff_l;
	Lapl_c1 = &(Lapl_cl->entry);
	Lapl_cl = Lapl_cl->next;
	Area_cl = n->Area_coeff_l;
	Area_c1 = &(Area_cl->entry);
	Area_cl = Area_cl->next;


	n0 = n1;
	Lapl_c0 = Lapl_c1;
	Area_c0 = Area_c1;
	WHILE (nl!=NULL) DO
		n2 = nl->entry;
		Lapl_c2 = &(Lapl_cl->entry);
		Area_c2 = &(Area_cl->entry);
		add_to_coefficients(n,n1,n2,Lapl_c1,Lapl_c2, Area_c1,Area_c2);

		Lapl_cl = Lapl_cl->next;
		Area_cl = Area_cl->next;

		nl = nl->next;
		n1 = n2;

		Lapl_c1 = Lapl_c2;
		Area_c1 = Area_c2;
	OD

	IF (!n->fixed) THEN
		add_to_coefficients(n,n1,n0, Lapl_c1,Lapl_c0,Area_c1, Area_c0);
	FI

END

	/* called by main.c  */

void setup_matrix_coefficients()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		set_coefficients_for_node(nl->entry);
		nl = nl->next;
	OD

END
