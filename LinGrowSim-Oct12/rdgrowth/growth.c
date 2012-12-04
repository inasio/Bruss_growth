#include "BM.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#include "triangle.h"
#define GLOBAL extern
#include "globals.h"
#include "setup_matrix.h"
#include "reaction_diffusion.h"
#include "random.h"
#include "init_distribution.h"

/*************
Will use the memory of arrays
BiCGSTAB_r, BiCGSTAB_p, BiCGSTAB_v, BiCGSTAB_s, BiCGSTAt_t, BiCGSTAB_phat, BiCGSTAB_shat;

for growth calculations, since these arrays need to be recalculated after
a growth step    12/14/99 - taking this out, I think it may be screwing up RD. 1/7/00 - put it back in, it was screwing up refine.c.
******************/

#define normalx		BiCGSTAB_r
#define normaly		BiCGSTAB_p
#define normalz		BiCGSTAB_v
#define deltax		BiCGSTAB_s
#define deltay		BiCGSTAB_t
#define deltaz		BiCGSTAB_phat
#define growthcount	BiCGSTAB_shat
#define maxnodes        50000


/*
 *
Global variables
	max_area_error     -- maximal error in area when approximating
	max_height_moved   -- maximal height above triangle plane to which
				node was moved
	max_node_distance  -- Maximal distance of nodes after growth step
 *
 */

double max_area_error;
double max_height_moved;
double max_node_distance;

/*
 *
Function
	normal_vector
Function normal_vector determines the coordinates of the normal vector
to the triangle given by its node arguments.

Arguments: three node pointers, three pointers to double
Returns: void
 *
 */

void normal_vector(node *n0, node *n1, node *n2, double *vx, double *vy, double *vz)
BEGIN
	double length, length1, length2, length3,  factor;
	double vxV, vyV, vzV,  ax,ay,az,  bx,by,bz,  mx,my,mz,
	      x0,x1,x2, y0,y1,y2, z0,z1,z2, X_value;

	x0 = n0->x; y0 = n0->y; z0 = n0->z;
	x1 = n1->x; y1 = n1->y; z1 = n1->z;
	x2 = n2->x; y2 = n2->y; z2 = n2->z;

	ax = x1 - x0;
	ay = y1 - y0;
	az = z1 - z0;

	bx = x2 - x0;
	by = y2 - y0;
	bz = z2 - z0;

	vxV = az*by - ay*bz;
	vyV = ax*bz - az*bx;
	vzV = ay*bx - ax*by;

	length = sqrt(vxV*vxV + vyV*vyV + vzV*vzV);

	factor =  1.0  / length;



	*vx = factor * vxV;
	*vy = factor * vyV;
	*vz = factor * vzV;
	/* (vx,vy,vz) is now the normal vector to the triangle pointing
	from the "inside" of our surface. *******************/
END

/*
 *
Function
	area_of_lifted_triangle

Function area_of_lifted_triangle takes three cartesian coordinates
and two node pointers. It calculates the area of the triangle
with the two nodes as specified and a third node with cartesian coordinates
as given.

Arguments: 3 doubles, Two pointers to node
Returns: Area of triangle
 *
 */

double area_of_lifted_triangle(double x1, double y1, double z1, node *n1, node *n2)
BEGIN
	double x2, x3, y2, y3, z2, z3;
       	double area, subterm;

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

/*
 *
Function
	area_of_element_with_lift

Function area_of_element_with_lift takes as arguments one node pointer,
three cartesian coordinates as double and one double "lift". It sums up
the areas of the triangles in the neighbourhood of the node "n"
when the node given  is moved from its
position by lift*the vector n (vector n has entries nx,ny,nz))

Arguments: one node pointer, four double
Returns: double - the area.
 *
 */

double area_of_element_with_lift(node *n, double nx, double ny, double nz, double lift)
BEGIN
	node_list *nl;
	node *n1;
	double area;
	double node_lifted_x = n->x+lift*nx;
	double node_lifted_y = n->y+lift*ny;
	double node_lifted_z = n->z+lift*nz;

	nl = n->nl;
	n1 = nl->entry;

	area = 0.0;
	WHILE (nl -> next != NULL) DO
		area += area_of_lifted_triangle(node_lifted_x, node_lifted_y, node_lifted_z,
						nl->entry, nl->next->entry);
		nl = nl -> next;
	OD

	area += area_of_lifted_triangle(node_lifted_x, node_lifted_y, node_lifted_z,
					nl->entry, n1);

	return(area);
END

/*
 *
Function
	set_average_normal_vector

Function set_average_normal_vector takes as argument one node pointer.
It takes the average of all vectors that are normal to the triangles
of which the node is a corner. Sets the arrays normalx,normaly,normalz
(see Macro definitions at top of file.)

Arguments: node pointer
Returns: void

 *
 */

void set_average_normal_vector(node *n)
BEGIN
	node_list *nbl;
	node *n1;
	double vx,vy,vz;
	double normal_x, normal_y, normal_z;
	double  num_triangle;

	nbl = n -> nl;
	n1 = nbl->entry;
	normal_x = 0.0; normal_y = 0.0; normal_z=0.0;

	WHILE (nbl->next != NULL) DO
		normal_vector(n, nbl->entry, nbl->next->entry, &vx,&vy,&vz);
		normal_x += vx; normal_y += vy; normal_z += vz;
		nbl = nbl->next;
	OD
	IF (! n->fixed) THEN
		normal_vector(n, nbl->entry, n1, &vx,&vy,&vz);
		normal_x += vx; normal_y += vy; normal_z += vz;
		num_triangle = n->degree;
	ELSE
		num_triangle = n->degree - 1;
	FI
	n->normalx = normal_x / num_triangle;
	n->normaly = normal_y / num_triangle;
	n->normalz = normal_z / num_triangle;
END

/*
 *
Function
	set_normals

Function set_normals calls set_normal_vector for each node.
 *
 */

void set_normals()
BEGIN
	node_list *nl;
	nl = first_nl;
	WHILE (nl != NULL) DO
		set_average_normal_vector(nl->entry);
		nl = nl -> next;
	OD
END

/*
 *
Function
	move_node

Function move_node changes the position of the node "n" given so that
the area of the element with center node "n" increases according to the
value of X at n.  The node is moved along the average of the normals of
the triangles which the node lies on.

It does this by measuring the area increase obtained by three different
(and hard-coded) movements, in order to guess the right movement, assuming
the area is a quadratic function of the "lift" (which is an approximation).

The approximation might fail when the squareroot of a negative number
is calculated, in which case an error message is written to stderr.

Arguments: node pointer
Returns: void
 *
 */
void move_node (node *n)
BEGIN
	node_list *nbl;
	node *n1;
	double denominator_a, denominator_b, denominator_c;
	double IV0, IV1, IV2;
	double coeff_a, coeff_b, coeff_c;
	double prescribed_area, rootarg;
	double vx, vy, vz;
	double length;
	double normal_x, normal_y, normal_z;
	double new_area, area_error;

	double approximated_length;

/*     fprintf(stderr,"growth 4.3.0\n"); */

/*	IF (n->fixed) THEN
		return;
		FI */

	/* should be n->X == 0.0 really. */
	IF (n->X <= 0.00001) THEN
		return;
	FI

	/* first establish the average normal vector for the collection of triangles
		on which the node n is incident */

	/*     fprintf(stderr,"growth 4.3.01\n"); */


	normal_x = n->normalx;
	normal_y = n->normaly;
	normal_z = n->normalz;

	length = normal_x * normal_x;
	length +=normal_y * normal_y;
	length +=normal_z * normal_z;

	length = sqrt(length);
	normal_x = normal_x / length;
	normal_y = normal_y / length;
	normal_z = normal_z / length;

/*     fprintf(stderr,"growth 4.3.1\n"); */



	/* Now have the normal vector. */

	/* Now find three area's for different lifts */
/* the interpolation points IP are hardcoded as follows */

#define IP0  0.0
#define IP1n  0.03    /* not used */
#define IP1  1e-5
#define IP2n  0.06    /* not used */
#define IP2  3e-5

/* The values at the interpolation points are */
	IV0 = area_of_element_with_lift(n, normal_x, normal_y, normal_z, IP0);
	IV1 = area_of_element_with_lift(n, normal_x, normal_y, normal_z, IP1);
	IV2 = area_of_element_with_lift(n, normal_x, normal_y, normal_z, IP2);

/* Now find coeff_a, coeff_b and coeff_c such that
	coeff_a * x*x + coeff_b * x + coeff_c = H
holds for x=IP0 and H=IV0
	  x=IP1 and H=IV1
	  x=IP2 and H=IV2	*/

	denominator_a = IP1*IP1*(IP2-IP0) + IP2*IP2*(IP0-IP1) + IP0*IP0*(IP1-IP2);
	denominator_b = (IP0-IP2) * (IP2-IP1) * (IP1-IP0);
	denominator_c = (IP0-IP2) * (IP0-IP1) * (IP1-IP2);

	coeff_a = ( IP0*(IV2-IV1) + IP1*(IV0-IV2) + IP2*(IV1-IV0) ) / denominator_a;
	coeff_b = ( IP0*IP0*(IV1-IV2) + IP1*IP1*(IV2-IV0) + IP2*IP2*(IV0-IV1) ) /
			denominator_b;
	coeff_c = ( IP0*IP0*(IP1*IV2-IP2*IV1) + IP1*IP1*(IP2*IV0-IP0*IV2) +
		             IP2*IP2*(IP0*IV1-IP1*IV0) ) / denominator_c;

/*     fprintf(stderr,"growth 4.3.2\n"); */


/*
 *
assuming IV0 is Area with 0 lift, so IP0 = 0.0
 *
 */

	prescribed_area = IV0*(1.0 + n->X* area_growth_factor*delta_t);

/*
 *
	Now have the values and determine x so that
	coeff_a * x*x + coeff_b*x + coeff_c = prescribed_area
	Call x approximated_length

 *
 */

	rootarg = coeff_b*coeff_b+4.0*coeff_a*(prescribed_area - coeff_c);

	IF (rootarg < 0.0) THEN
		fprintf(stderr, "SRD_Growth[%d]: Growth step failed with complex square root!\n",pid);
                user_stops_exe = TRUE;
                exestopmsg = "complex sqrt in growth";
		return;
	FI

	/*     fprintf(stderr,"growth 4.3.3\n"); */


	approximated_length = 0.5 * (-coeff_b + sqrt(rootarg))/coeff_a;

    /* HARD STOP TO BLOWUPS */

      if(approximated_length > 0.025)
      {
         approximated_length = 0.025;
      }    

        if(approximated_length < 0.0)
        {
  	  approximated_length = 0.5 * (-coeff_b - sqrt(rootarg))/coeff_a;
          fprintf(stderr,"chose neg. root in growth, %d\n", n->id);
	}



	new_area = area_of_element_with_lift(n, normal_x, normal_y, normal_z,
					approximated_length);

	area_error = new_area - prescribed_area;

	IF (area_error < 0.0) THEN area_error = - area_error; FI

	/*     fprintf(stderr,"growth 4.3.4\n"); */

	/*
	 *
	Keep track for tracing by user
	 *
 	 */

	IF (area_error > max_area_error) THEN
		max_area_error = area_error;
	FI
	IF (max_height_moved < approximated_length) THEN
		max_height_moved=approximated_length;
	FI


	n->deltax += approximated_length * normal_x;
	n->deltay += approximated_length * normal_y;
	n->deltaz += approximated_length * normal_z;
END

/*
 *
Function
	move_nodes_with_probability

Function move_nodes_with_probability calls move_node for each
node for which a coin toss said so, and which was not
moved previously in the growth step (growthcount == 0).

Arguments: float, giving probability below which node is lucky
Returns: void
 *
 */
void move_nodes_with_probability()
BEGIN
	node_list *nl;
        int j;
        int i,k;
        float dist;
        FILE *save;

       struct node_s *varr[number_of_nodes];  /*definition of an array
           to hold all nodes. struct definition needs node_s, not node. */


/*      fprintf(stderr,"growth 4.1\n"); */

        save = fopen("./n.dat","w");
        nl = first_nl;
        for(i=0;i<number_of_nodes;i++)     /*load up the varr array */
        {
           varr[i]=nl->entry;
           fprintf(save,"%ld,%d,%d\n",iterations,varr[i]->id,i);
	   /*           fprintf(stderr,"%d,%d,%d\n",iterations,varr[i]->id,i); */
           nl = nl->next;
	}


      fclose(save);
/*      fprintf(stderr,"growth 4.2\n"); */


/* note: float to int conversion causes truncation of decimal
     (always rounds down). */

        while (number_of_nodes > 0)
        {
           k=number_of_nodes*ran();
           if(k==number_of_nodes) k--;

	   /*	   fprintf(stderr,"growth 4.3, %d,%d,%f\n",number_of_nodes,k,ran()); */
	   /*		   fprintf(stderr,"%f,%f\n",varr[k]->deltax,varr[k]->X);  */


              varr[k]->deltax = varr[k]->x * 0.0;
              varr[k]->deltay = varr[k]->y * 0.0;
              varr[k]->deltaz = varr[k]->z * 0.0;

              varr[k]->deltax *= delta_t;
              varr[k]->deltay *= delta_t;
              varr[k]->deltaz *= delta_t;


	      move_node(varr[k]);

            varr[k]->x +=varr[k]->deltax;
	    varr[k]->y +=varr[k]->deltay;
	    varr[k]->z +=varr[k]->deltaz;

dist = sqrt((varr[k]->x*varr[k]->x)+(varr[k]->y*varr[k]->y)+(varr[k]->z*varr[k]->z));
	    if (dist>max_node_distance) max_node_distance=dist;

	    /*           fprintf(stderr,"growth 4.6 %d\n",number_of_nodes); */


           number_of_nodes--;

          if(number_of_nodes==0) return;


           for(j=k;j<number_of_nodes;j++)
	   {
              varr[j]=varr[j+1];
	   }
	   /*           fprintf(stderr,"growth 4.7 %d\n",number_of_nodes); */

	}

/*        fclose(save);  */
        return;

	   /*           nl = first_nl;
           while (nl!=0)
	   {
             node *n = nl->entry;
             if(n->growthcount == 0 && n->id = z)
	     {
                move_node(n);
                n->growthcount=1;
                j++;
                break;
	     }
             nl = nl->next;
	     } */


/* end of 11/4, This should be a way to fire through all the nodes sequentially,
    once for each node move. This method looked through the node list sequentially to find
   each node for a match of n->id with the random number. 1) There should be a way to find a
    new random number and look for a match without going back to first_nl. 2) Hopefully
      there is a way to  address nodes by their i.d., without sequential searching of
       the node lists. This would be like my approach in 2D - which depended on writing out
        the sequence of integers 0-n, choosing one at random, and working with the
          variables (position, X, etc.) at that address. The problem with structs seems to
            be that the i.d. is buried in the node_list, and the node must be addressed
              before the i.d. can be gotten. (11/5 - see above for present fix, using an
                  array of structs. */

/*	Old loop of Stephan's.
        nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		IF (n->growthcount == 0.0) THEN
			IF (ran() < prob) THEN
				move_node(n);
				n->growthcount = 1.0;
			FI
		FI
		nl = nl->next;
		OD */
END

/*
 *
Function
	one_growth_step

Function one_growth_step moves nodes with a number of probabilities, then
it moves all "unlucky" nodes.
Each node is moved exactly once per growth_step!

Is not called within the package. Just included to show a possibility.

Arguments: none
Returns: void
 *
 */
/*   void one_growth_step()
  BEGIN
	node_list *nl;

	move_nodes_with_probability(0.0625);
	move_nodes_with_probability(0.125);
	move_nodes_with_probability(0.25);
	move_nodes_with_probability(0.5);


	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		IF (n->growthcount == 0.0) THEN
			move_node(n);
		ELSE
			n->growthcount = 0.0;
		FI
		nl = nl->next;
	OD
	END   */

/*
 *
Function
	growth

Function growth calls move_nodes for each node moves_per_growth_step
many times.

It calls setup_matrix_coefficients from setup_matrices.c and
set_RK_matrix_coefficients so that  Runge-Kutta step can be performed.

The function outputs some statistics to stdout.

Arguments: none
Returns: void
 *
 */
void growth ()
BEGIN
	node_list *nl;
	short i;

max_area_error = -0.1;
max_height_moved = -0.1;
max_node_distance = 0;


fprintf(stderr, "Growth ");


 	FOR (i=moves_per_growth_step; i > 0; i--) DO
fprintf(stderr, "\rGrowth %d ", i);


			set_normals();

                        number_of_nodes = 0;

			nl = first_nl;
			WHILE (nl != NULL) DO
				node *n=nl->entry;
				n->deltax = 0.0;
				n->deltay = 0.0;
				n->deltaz = 0.0;
                                number_of_nodes++;
				nl=nl->next;
			OD
			/*        fprintf(stderr,"growth 4\n"); */


	    /*  Following line calls random visitation routine. */

		        move_nodes_with_probability();


			/* Following block needed for simultaneous update */
			/*		      	nl = first_nl;
			WHILE (nl !=  NULL) DO
				move_node(nl->entry);
				nl = nl ->next;
				OD  */



/* Following block, without previous block, gives sequential visitation
       with continuous update of nodes. */

				/*	      	       nl = first_nl;
		       WHILE (nl != NULL) DO
			 node *n=nl->entry;
			 double dist;
                           move_node(n);
			   n->x += n->deltax;
			   n->y += n->deltay;
			   n->z += n->deltaz;
			   dist = sqrt( (n->x*n->x)+ (n->y*n->y) +
					(n->z*n->z));
			   if (dist>max_node_distance) max_node_distance=dist;
			   nl=nl->next;
			   OD */
	OD

	/*	setup_matrix_coefficients();
		set_RK_matrix_coefficients(0); */

	/*        fprintf(stderr,"growth 5\n"); */

		nl = first_nl;
		WHILE (nl != NULL) DO
		   node *n=nl->entry;
                     set_coefficients_for_node(n);
                     set_for_node_RK_matrix_coefficients_0(n);
		   nl=nl->next;
                OD

fprintf(stderr, " MaxAreaIncrErr=%g at max_height_moved=%g\n        max_node_distance = %g\n", max_area_error,
max_height_moved, max_node_distance);

	IF (max_height_moved > 20.0) THEN
		fprintf(stderr, "SRD_Growth[%d]: surface exploding\n",pid);
                user_stops_exe = TRUE;
                exestopmsg = "mesh explosion";
		return;
	FI

END

