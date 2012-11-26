#include "real.h"

//typedef struct node_s node;

//typedef struct node_list_s node_list;

#define IDENTIFY_NODES_AND_TRIANGLES


struct node {
	REAL 		x,y,z;	/* coordinates */

	struct node_list       *nl;    /* the nodes neighbours */

	short 		degree; /* Number of triangles incident on the node */
	short 		fixed;	/* flag indicating, whether node can be moved */
	short 		conc_fixed;	/* flag indicating, whether morphogen can be updated */


	REAL		X, Y, A;
#ifdef 	IDENTIFY_NODES_AND_TRIANGLES
	int 		id;		 /* For identification! */
#endif
};
struct node_list {
	node		*entry;
	node_list	*next;
};

