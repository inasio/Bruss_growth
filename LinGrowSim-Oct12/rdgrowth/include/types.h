#include "real.h"

typedef struct node_s node;

typedef struct node_list_s node_list;


typedef struct coeff_list_s coeff_list;

#define IDENTIFY_NODES_AND_TRIANGLES


struct node_s {
	double 		x,y,z;	/* coordinates */
	double		local_x, local_y;

	node_list 	*nl;    /* the nodes neighbours */

	short 		degree; /* Number of triangles incident on the node */
	short 		fixed;	/* flag indicating, whether node can be moved */
        short           conc_fixed;  /* flag indicating whether concentration can be changed */

	double		X, Y, A;
	double 		previous_X, previous_Y, previous_A;


	coeff_list 	*Lapl_coeff_l;
	double		Lapl_coeff;

	coeff_list	*Area_coeff_l;
	double		Area_coeff;

       coeff_list      *Matrix_coeff_l_X, *Matrix_coeff_l_Y, *Matrix_coeff_l_A;
	double		Matrix_coeff_X, Matrix_coeff_Y, Matrix_coeff_A;


	double		solution_X, solution_Y, solution_A;
	double 		rhs_X, rhs_Y, rhs_A;

	double		*K_X, *K_Y, *K_A;
	double		*Khat_X, *Khat_Y, *Khat_A;
	double		*u_X, *u_Y, *u_A;

	double		tempdouble_X, tempdouble_Y, tempdouble_A;

	double		BiCGSTAB_r, BiCGSTAB_p, BiCGSTAB_v, BiCGSTAB_s, BiCGSTAB_t, BiCGSTAB_phat,
			BiCGSTAB_shat;

	double		deltaX, deltaY, deltaA;

#ifdef 	IDENTIFY_NODES_AND_TRIANGLES
	int 		id;		 /* For identification! */
#endif
};

struct node_list_s {
	node		*entry;
	node_list	*next;
};


struct coeff_list_s {
	double 		entry;
	coeff_list 	*next;
};













