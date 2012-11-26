#include "BM.h"
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "random.h"

#define GLOBAL extern
#include "globals.h"


void add_noise_at_node(node *n)
BEGIN
	float r;

/* should be giving the same noise as I had in 2D Micrasterias programs:
    uniform deviations up to 1 Poisson s.d. No larger magnitude noise
    allowed. */


	IF ((n->X < 0.15) || (n->Y < 0.15)) THEN
		return;
		FI

	r = ran();
	r = 1.0 - 2.0*r;
	r /= sqrt(10000*n->X);

	n->X += r;
        if(n->X < 0.0 || n->X > 100)
        {
      /*                printf("addnoise,X is %f, id is %d\n",n->X,n->id);*/
	        user_stops_exe = TRUE;
                exestopmsg = "X negative or too large";
		return;
		/*                n->Y = 0.0; */
	}

        if((n->X <= Xth) && (iterations > growth_onset)) n->conc_fixed = TRUE; 
 

	r = ran();
	r = 1.0 - 2.0*r;
	r /= sqrt(10000*n->Y);

	n->Y += r;
        if(n->Y < 0.0 || n->Y > 100)
        {
  /*                printf("addnoise,Y is %f, id is %d\n",n->Y,n->id);*/
	        user_stops_exe = TRUE;
                exestopmsg = "Y negative or too large";
		return;
		/*                n->Y = 0.0; */
	}

/*	r = ran();
	r = 1.0 - 2.0*r;
	r /= sqrt(10000*n->A);

	n->A += r;
        if(n->A < 0.0)
        {
                printf("addnoise,A is %f, id is %d\n",n->A,n->id);
	        user_stops_exe = TRUE;
		return;
	} */
END

void add_noise()
BEGIN
	node_list *nl;
	nl = first_nl;
	WHILE (nl != NULL) DO

           node *n = nl->entry;

           if(!n->conc_fixed)
           {
		add_noise_at_node(n);
	   }
		nl=nl->next;
	OD
END

REAL f_at_node (node *n)
BEGIN
	REAL x,y,z,  R;
	x = n->x; y = n->y; z = n->z;
	R = sqrt(x*x + y*y + z*z);
	return ((x/R) * (y/R) * (z/R)); /* R never zero! */
END

REAL minimum_f;
void find_minimum_f()
BEGIN
	node_list *nl;

	minimum_f = 1e4;
	nl = first_nl;
	WHILE (nl != NULL) DO
		REAL f_node;
		node *n = nl->entry;
		n->X = f_node = f_at_node(n);
		IF (f_node < minimum_f) THEN minimum_f = f_node; FI
		nl = nl -> next;
	OD
END

void initialize(node *n)
BEGIN
	REAL x = n->x,  y=n->y, z=n->z;
	REAL rsquared = x*x + y*y;

/* A prepatterns */

/*  annulus */
/*  n->A = 2*((-(5*pow(z,3)-3*z*pow(radius,2))/pow(radius,3))+3.0);  */

/* tip growth  */

/*   if(rsquared <= radius*radius)
   {
      n->A = initA*(sqrt(1-(rsquared/(radius*radius)))) + 1.0;
   }
   else
   {
     n->A = 1.0;
     }  */




/* homogeneous */
   n->A = initA;  

/* Dicot  */

/*     n->A = initA - 22.5*(((-0.5/3.8)*(5*pow(z,3)-3*z*pow(radius,2))/pow(radius,3))+(0.5*(z*(x*x-y*y)/pow(radius,3)))); */


    /*       n->Y = (-0.375*(5*pow(z,3)-3*z*pow(radius,2))/pow(radius,3))
       +(1.3263*(z*(x*x-y*y)/pow(radius,3)));  */



/* A-stripe gradient, put in (laptop) 12/16/03 */

/*     if(n->y > -0.05 && n->y < 0.05)
     {
        n->A += 8.0;
	} */ 

/* Homogeneous, steady-state initial concentrations for Brusselator X, Y */

/*       n->A = (a/Kd)*((Kp*Su/d)-1); */
       n->X = a*n->A/d;
       n->Y = b*B/(c*n->X);

      if(!n->fixed)  add_noise_at_node(n); 


/* Make ellipsoid from hemisphere, added (laptop) 12/16/03 */

/*       n->x *= 2.0; */


/* This is the spherical harmonic for the morphogen patterns. */

/* Y3,2

      n->X = z*(x*x-y*y)/pow(radius,3);
      n->Y = z*(x*x-y*y)/pow(radius,3);  */


/* Tip Growth (Y1,0), cuts off growth for z < equator */


/*     if((x*x+y*y) >= (radius*radius))
     {
        n->X=0.0;
        n->Y=0.0;
     }

     else
     {
       n->X = sqrt(1-((x*x+y*y)/(pow(radius,2))));
       n->Y = sqrt(1-((x*x+y*y)/(pow(radius,2))));
       }  */

/*  Monocot

       n->X = y*z/pow(radius,2);
       n->Y = y*z/pow(radius,2);  */


/* Annulus (Y3,0)

        n->X = -(5*pow(z,3)-3*z*pow(radius,2))/pow(radius,3);
	n->Y = -(5*pow(z,3)-3*z*pow(radius,2))/pow(radius,3);  */




END

void init_distribution(void)
BEGIN

        FILE *save;
        int k;

	node_list *running_node;



	t0 = 0.0;

         save = fopen("./initvals.dat","w");

/*	max_X_value = max_Y_value = 6.0;
	min_X_value = min_Y_value = 0.0; */

	find_minimum_f();
	running_node = first_nl;

	WHILE (running_node != NULL) DO
		node *n;
		n = running_node -> entry;
		initialize(n);


                fprintf(save,"%d,%f,%f,%f\n",n->id,n->X,n->Y,n->A);

/*		IF (n -> X  >  max_X_value) THEN max_X_value = n -> X; FI
		IF (n -> X  <  min_X_value) THEN min_X_value = n -> X; FI
		IF (n -> Y  >  max_Y_value) THEN max_Y_value = n -> Y; FI
		IF (n -> Y  <  min_Y_value) THEN min_Y_value = n -> Y; FI */

		running_node = running_node->next;
	OD


        fclose(save);

END






