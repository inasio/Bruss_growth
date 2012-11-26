#include "BM.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#define GLOBAL extern
#include "globals.h"
#include "setup_matrix.h"
#include "solve_system.h"

/***********************    third-order combination page 8. ****************

#define S 2

#define gamma 0.78867513459481288227
#define commentgamma  gamma is (3+sqrt(3)) / 6

REAL imex_a[S][S] = {
	{ gamma,		0.0},
	{ 1.0-2.0*gamma,	gamma}
      };

REAL imex_ahat[S+1][S+1] = {
	{ 0.0,		0.0,		0.0},
	{ gamma,	0.0,		0.0},
	{ gamma,	2.0*(1.0-gamma), 0.0}
      };

REAL imex_b[S]    = {0.5, 0.5};
REAL imex_bhat[S+1] = {0.0, 0.5, 0.5};

*****************************************************************************/
/****************** third-order combination page 9/10 ****************/

#define S 3

#define a_ii_do_not_vary

REAL imex_a[S][S] = {
{ 0.4358665215,		0.0,		0.0},
{ 0.2820667392,		0.4358665215,   0.0},
{ 1.208496649,	       -0.644363171,	0.4358665215} };

REAL imex_ahat[S+1][S+1] = {
{ 0.0,			0.0,		0.0,		0.0},
{ 0.4358665215,		0.0,		0.0,		0.0},
{ 0.3212788860,		0.3966543747,	0.0,		0.0},
{ -0.105858296,		0.5529291479,	0.5529291479,	0.0}};

REAL imex_b[S] = {1.208496649, -0.644363171, 0.4358665215};
REAL imex_bhat[S+1] = {0.0, 1.208496649, -0.644363171, 0.4358665215};
/**********************************************************************/

/* second order combination page 7 **********************************
#define S 1
#define a_ii_do_not_vary

REAL imex_a[S][S] = { {0.5} };
REAL imex_ahat[S+1][S+1] = {
{0.0, 0.0},
{0.5, 0.0}};

REAL imex_b[S] = {1.0};
REAL imex_bhat[S+1] = {0.0, 1.0};
********************************************************************/

void allocate_imex_arrays_for_node(node *n)
BEGIN
	n->K_X    = malloc( S * sizeof(REAL));
	n->Khat_X = malloc( (S+1) * sizeof(REAL));
	n->u_X    = malloc( S * sizeof(REAL));

	n->K_Y    = malloc( S * sizeof(REAL));
	n->Khat_Y = malloc( (S+1) * sizeof(REAL));
        n->u_Y    = malloc( S * sizeof(REAL));

	n->K_A    = malloc( S * sizeof(REAL));
	n->Khat_A = malloc( (S+1) * sizeof(REAL));
        n->u_A    = malloc( S * sizeof(REAL));

END

void initialize_imex_arrays()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl -> entry;
		allocate_imex_arrays_for_node(n);
		nl = nl->next;
	OD

END


void set_previous_values()
BEGIN
	node_list *nl;
	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		n->previous_X = n->X;
		n->previous_Y = n->Y;
		n->previous_A = n->A;
		nl = nl->next;
	OD
END

void set_Khat_1()
BEGIN
	node_list *nl;
	nl = first_nl;

	WHILE (nl != NULL) DO
		node *n = nl->entry;
		REAL X = n->previous_X,
                     Y = n->previous_Y,
                     A = n->previous_A;


              if(!n->fixed)
		{ 

/****************** explicit ***************************/

/*	if(n->X > Xth)
        { */
          n->Khat_X[0] =  a*A - b*B*X + c*X*X*Y - d*X;
	  n->Khat_Y[0] =  b*B*X - c*X*X*Y;
          n->Khat_A[0] = 0.0;  
	  /*	}
        else 
        {
          n->Khat_X[0] = -d*X;
          n->Khat_Y[0] = 0.0;
          n->Khat_A[0] = 0.0;
	  }*/ 
/****************** explicit **************************/
		}
           else
	   {

          n->Khat_X[0] = 0.0;
          n->Khat_Y[0] = 0.0;
          n->Khat_A[0] = 0.0;

	  	  }
		nl = nl->next;
	OD
END


/* Following called after a refine or growth step */
void set_for_node_RK_matrix_coefficients_0(node *n)
BEGIN
	int i = 0;
	REAL delta_t_a_ii = delta_t * imex_a[i][i];

	coeff_list *Acl = n->Area_coeff_l;
	coeff_list *Lcl = n->Lapl_coeff_l;
	coeff_list *MclX = n->Matrix_coeff_l_X;
	coeff_list *MclY = n->Matrix_coeff_l_Y;
	coeff_list *MclA = n->Matrix_coeff_l_A;

	n->Matrix_coeff_X=n->Area_coeff - delta_t_a_ii*X_diffuse*n->Lapl_coeff;
	n->Matrix_coeff_Y=n->Area_coeff - delta_t_a_ii*Y_diffuse*n->Lapl_coeff;
	n->Matrix_coeff_A=n->Area_coeff - delta_t_a_ii*A_diffuse*n->Lapl_coeff;

	WHILE (MclX != NULL) DO
	/*      	 if(MclX != NULL)
			 {   */
           MclX->entry = Acl->entry - delta_t_a_ii * X_diffuse * Lcl->entry;
/*	 }
      	 if(MclY != NULL)
         {  */
	   MclY->entry = Acl->entry - delta_t_a_ii * Y_diffuse * Lcl->entry;
	   MclA->entry = Acl->entry - delta_t_a_ii * A_diffuse * Lcl->entry;
	   MclX = MclX->next;
	   MclY = MclY->next;
	   MclA = MclA->next;
/*	 } */
/*      	 if(MclA != NULL)
         {
	   MclA->entry = Acl->entry - delta_t_a_ii * A_diffuse * Lcl->entry;
	   MclA = MclA->next;
	   } */

	 Acl = Acl->next;
	 Lcl = Lcl->next;
	OD
END


/* Following called from main.c  */
void set_RK_matrix_coefficients(short i)
BEGIN
	node_list *nl;
	REAL delta_t_a_ii;

	nl = first_nl;
	delta_t_a_ii = delta_t * imex_a[i][i];

	WHILE (nl != NULL) DO
		node *n = nl->entry;
		coeff_list *Acl = n->Area_coeff_l;
		coeff_list *Lcl = n->Lapl_coeff_l;
		coeff_list *MclX = n->Matrix_coeff_l_X;
		coeff_list *MclY = n->Matrix_coeff_l_Y;
		coeff_list *MclA = n->Matrix_coeff_l_A;

		n->Matrix_coeff_X = n->Area_coeff - delta_t_a_ii * X_diffuse * n->Lapl_coeff;
		n->Matrix_coeff_Y = n->Area_coeff - delta_t_a_ii * Y_diffuse * n->Lapl_coeff;
         	n->Matrix_coeff_A = n->Area_coeff - delta_t_a_ii * A_diffuse * n->Lapl_coeff;

		WHILE (MclX != NULL) DO
			MclX->entry = Acl->entry - delta_t_a_ii * X_diffuse * Lcl->entry;
			MclY->entry = Acl->entry - delta_t_a_ii * Y_diffuse * Lcl->entry;
			MclA->entry = Acl->entry - delta_t_a_ii * A_diffuse * Lcl->entry;

			MclX = MclX->next;
			MclY = MclY->next;
			MclA = MclA->next;
			Acl = Acl->next;
			Lcl = Lcl->next;
		OD

		nl = nl->next;
	OD
END

void solve_Ki_eq_gui(short i)
BEGIN
	/* First set right handside of linear system.*/

	node_list *nl;
	REAL delta_t_a_ii;

/*        FILE *save;
	  node_list *running_node; */
   
	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl -> entry;
		short j;
		n->rhs_X = n->previous_X;
		n->rhs_Y = n->previous_Y;
		n->rhs_A = n->previous_A;


		j = 0;

		WHILE (j <= i-1) DO
			n->rhs_X += delta_t * imex_a[i][j] * n->K_X[j];
			n->rhs_Y += delta_t * imex_a[i][j] * n->K_Y[j];
			n->rhs_A += delta_t * imex_a[i][j] * n->K_A[j];
			j ++;
		OD

		j=0;
		WHILE (j <= i) DO
			n-> rhs_X += delta_t * imex_ahat[i+1][j] * n->Khat_X[j];
			n-> rhs_Y += delta_t * imex_ahat[i+1][j] * n->Khat_Y[j];
			n-> rhs_A += delta_t * imex_ahat[i+1][j] * n->Khat_A[j];
			j++;

		OD

		nl = nl->next;
	OD


	/* Now multiply the vector n->rhs by Lapl-matrix. */


        nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		coeff_list *cl = n->Lapl_coeff_l;
		node_list *nbl = n->nl;

		REAL value_X = X_diffuse * n->Lapl_coeff * n->rhs_X;
		REAL value_Y = Y_diffuse * n->Lapl_coeff * n->rhs_Y;
		REAL value_A = A_diffuse * n->Lapl_coeff * n->rhs_A;

 
		WHILE (nbl != NULL) DO
			value_X += X_diffuse * cl->entry * nbl->entry->rhs_X;
			value_Y += Y_diffuse * cl->entry * nbl->entry->rhs_Y;
			value_A += A_diffuse * cl->entry * nbl->entry->rhs_A;

			nbl = nbl->next;
			cl  = cl ->next;
		OD
		n->tempREAL_X = value_X;
		n->tempREAL_Y = value_Y;
		n->tempREAL_A = value_A;

		nl = nl->next;
 	OD


	nl = first_nl;
	WHILE(nl != NULL) DO
		node *n = nl->entry;
		n->rhs_X = n->tempREAL_X;
		n->rhs_Y = n->tempREAL_Y;
		n->rhs_A = n->tempREAL_A;

		nl = nl->next;
	OD


	/* Now rhs contains Lapl*(u_n-1 + delta_t*sum(a_ij*K_j) + delta_t*sum(ahat_i+1,j*Khat_j) */

	/* Set Matrix of linear system : */

#ifdef a_ii_vary
	set_RK_matrix_coefficients(i);
#endif
	/* Now have set up the  linear system */

/*         printf("in solve Ki eq gui, before solve system\n");  */


	solve_system();

	/* copy result into K_i - vector */


	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;

		n->K_X[i] = n->solution_X;
		n->K_Y[i] = n->solution_Y;
		n->K_A[i] = n->solution_A;

		nl = nl->next;
	OD


	/*         printf("in solve Ki eq gui, after K's - end\n"); */
END

void set_ui (short i)
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		short j;
		n->u_X[i] = n->previous_X;
		n->u_Y[i] = n->previous_Y;
		n->u_A[i] = n->previous_A;

		j = 0;
		WHILE (j <= i) DO
			n->u_X[i] += delta_t * imex_a[i][j] * n->K_X[j] +
				     delta_t * imex_ahat[i+1][j] * n->Khat_X[j];
			n->u_Y[i] += delta_t * imex_a[i][j] * n->K_Y[j] +
				     delta_t * imex_ahat[i+1][j] * n->Khat_Y[j];
			n->u_A[i] += delta_t * imex_a[i][j] * n->K_A[j] +
                	             delta_t * imex_ahat[i+1][j] * n->Khat_A[j];
			j ++;
		OD

		nl = nl->next;
	OD
END


void set_Khat_i_plus_1(short i)
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE(nl != NULL) DO
		node *n = nl -> entry;
		REAL X = n->u_X[i],
                     Y = n->u_Y[i],
	             A = n->u_A[i];
              if(!n->fixed)
		{ 

/****************** explicit **************************/
/*		if(n->X > Xth)
                { */ 
                  n->Khat_X[i+1] =  a*A - b*B*X + c*X*X*Y - d*X;
  	     	  n->Khat_Y[i+1] =  b*B*X - c*X*X*Y;
                  n->Khat_A[i+1] =  0.0; 
		  /*		}
                else 
                {
                  n->Khat_X[i+1] = -d*X;
                  n->Khat_Y[i+1] =  0.0;
                  n->Khat_A[i+1] =  0.0;
		  } */
/****************** explicit **************************/
		} 
              else
              {
                  n->Khat_X[i+1] =  0.0;
                  n->Khat_Y[i+1] =  0.0;
                  n->Khat_A[i+1] =  0.0;
	      }

		nl = nl -> next;
	OD
END

void set_new_XY_values()
BEGIN

	node_list *nl;

/*	min_X_value = min_Y_value = min_A_value = 1e10;
	max_X_value = max_Y_value = max_A_value = 0.0; */


	nl = first_nl;

	WHILE (nl != NULL) DO
		node *n = nl->entry;
		short j;

		n->X = n->previous_X;
		n->Y = n->previous_Y;
		n->A = n->previous_A;

/* Dirichlet BC, don't update fixed points (equatorial).
   Neumann BC, update all points - diffusion already works this way,
             I think (3/22/00) */


              if(!n->fixed)
		{ 
		  FOR (j=0; j<S; j++) DO
			n->X += delta_t * imex_b[j] * n->K_X[j];
			n->Y += delta_t * imex_b[j] * n->K_Y[j];
	                n->A += delta_t * imex_b[j] * n->K_A[j];
		  OD
		  FOR (j=0; j< S+1; j++) DO
			n->X += delta_t * imex_bhat[j] * n->Khat_X[j];
			n->Y += delta_t * imex_bhat[j] * n->Khat_Y[j];
	                n->A += delta_t * imex_bhat[j] * n->Khat_A[j];
		  OD
		 	} 


/*		IF (max_X_value < n->X) THEN max_X_value = n->X; FI
		IF (max_Y_value < n->Y) THEN max_Y_value = n->Y; FI
		IF (min_X_value > n->X) THEN min_X_value = n->X; FI
		IF (min_Y_value > n->Y) THEN min_Y_value = n->Y; FI */

          if(n->X < 0.01)
          {
	                 n->X = n->previous_X;
			 /*       printf("X not reset: X %f, Y %f, A %f, id %d\n",n->X,n->Y,n->A,n->id);   */
	 /*	        user_stops_exe = TRUE;
			return; */
	  }

          if(n->Y < 0.01)
          {
                       n->Y = n->previous_Y;
		       /*       printf("Y not reset: Y %f, X %f, A %f, id %d\n",n->Y,n->X, n->A,n->id);  */
		/*	        user_stops_exe = TRUE;
				return;  */
       	  }

          if(n->A < 0.01)
          {
	        n->A = n->previous_A;
		/*             printf("A not reset: A is %f, id is %d\n",n->A,n->id);   */
	     /*	        user_stops_exe = TRUE;
			return;  */
	   }

/* I turned off conc_fixed flagging  7/17/03  
        if((n->X <= Xth) && (iterations > growth_onset)) 
        {
           n->conc_fixed = TRUE;
           n->A = (a/Kd)*((Kp*Su/d)-1);
           n->X = a*n->A/d;
           n->Y = b*B/(c*n->X);
	   }*/


		nl = nl->next;
	OD
END

void reaction_diffusion(void)
BEGIN
	REAL t = t0 + iterations*delta_t;
	short i;

	set_previous_values();

	set_Khat_1();

 	FOR (i=0; i<S; i++) DO
		solve_Ki_eq_gui(i);
		set_ui(i);
		set_Khat_i_plus_1(i);

	OD

	set_new_XY_values();
END





/****************** third-order combination page 10
**************************************************
#define S 4

REAL imex_a[S][S] = {
  { 0.5,	0.0,	0.0,	0.0},
  { 1.0/6.0,	0.5,	0.0,	0.0},
  { -0.5,	0.5,	0.5,	0.0},
  { 1.5,	-1.5,	0.5,	0.5}
};

REAL imex_ahat[S+1][S+1] = {
  { 0.0,	0.0,	0.0,	0.0,	0.0},
  { 0.5,	0.0,	0.0,	0.0,	0.0},
  { 11.0/18.0,	1.0/18.0, 0.0,	0.0,	0.0},
  { 5.0/6.0,	-5.0/6.0,	0.5,	0.0},
  { 0.25,	1.75,	0.75,	-0.75,  0.0}
};
REAL imex_b[S] = {1.5, -1.5, 0.5, 0.5};
REAL imex_bhat[S+1] = {0.25, 1.75, 0.75, -1.75, 0.0};
******************************************************************/





















