#include "real.h"

typedef struct node_s node;

typedef struct node_list_s node_list;


typedef struct coeff_list_s coeff_list;

#define IDENTIFY_NODES_AND_TRIANGLES


struct node_s {
	REAL 		x,y,z;	/* coordinates */
	REAL		local_x, local_y;

	node_list 	*nl;    /* the nodes neighbours */

	short 		degree; /* Number of triangles incident on the node */
	short 		fixed;	/* flag indicating, whether node can be moved */
        short           conc_fixed;  /* flag indicating whether concentration can be changed */

	REAL		X, Y, A;
	REAL 		previous_X, previous_Y, previous_A;


	coeff_list 	*Lapl_coeff_l;
	REAL		Lapl_coeff;

	coeff_list	*Area_coeff_l;
	REAL		Area_coeff;

       coeff_list      *Matrix_coeff_l_X, *Matrix_coeff_l_Y, *Matrix_coeff_l_A;
	REAL		Matrix_coeff_X, Matrix_coeff_Y, Matrix_coeff_A;


	REAL		solution_X, solution_Y, solution_A;
	REAL 		rhs_X, rhs_Y, rhs_A;

	REAL		*K_X, *K_Y, *K_A;
	REAL		*Khat_X, *Khat_Y, *Khat_A;
	REAL		*u_X, *u_Y, *u_A;

	REAL		tempREAL_X, tempREAL_Y, tempREAL_A;

	REAL		BiCGSTAB_r, BiCGSTAB_p, BiCGSTAB_v, BiCGSTAB_s, BiCGSTAB_t, BiCGSTAB_phat,
			BiCGSTAB_shat;

	REAL		deltaX, deltaY, deltaA;

#ifdef 	IDENTIFY_NODES_AND_TRIANGLES
	int 		id;		 /* For identification! */
#endif
};

struct node_list_s {
	node		*entry;
	node_list	*next;
};


struct coeff_list_s {
	REAL 		entry;
	coeff_list 	*next;
};













