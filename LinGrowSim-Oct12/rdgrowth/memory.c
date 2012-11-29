#include "BM.h"

#include "types.h"
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#define GLOBAL extern
#include "globals.h"

#define BLOCKS_TO_PRELOAD	4048


int free_nodes          = 0,
    free_node_lists     = 0,
    free_coeff_lists    = 0;


#ifdef IDENTIFY_NODES_AND_TRIANGLES
int   node_id = 0;
#endif

node 	 	*next_node;
node_list 	*next_node_list;
coeff_list 	*next_coeff_list;


void set_node_id(int v)
BEGIN
	node_id = v;
END


node *new_node_element()
BEGIN
	node *return_node;


	IF (free_nodes > 0) THEN
		return_node = next_node;
		next_node ++;
		free_nodes --;

		return_node->fixed=FALSE;
                return_node->conc_fixed=FALSE;

		return_node->Lapl_coeff_l = NULL;
		return_node->Area_coeff_l = NULL;
		return_node->Matrix_coeff_l_X = NULL;
		return_node->Matrix_coeff_l_Y = NULL;
		return_node->Matrix_coeff_l_A = NULL;

#ifdef IDENTIFY_NODES_AND_TRIANGLES
		return_node->id = node_id++;
/*                printf("new id is %d\n",return_node->id);  */
#endif
		return_node -> nl = NULL;
		return(return_node);
	ELSE
		next_node = malloc(BLOCKS_TO_PRELOAD * sizeof(node));
		IF (next_node == NULL) THEN
			error_code = OUT_OF_MEMORY;
			fprintf(stderr, "SRD_Growth[%d]: Out of memory\n", pid);
			EXIT;
		FI
		free_nodes = BLOCKS_TO_PRELOAD;
		return(new_node_element());   /** a recursion! **/
	FI
END

node_list *new_node_list()
BEGIN
	node_list *return_node_list;
	IF (free_node_lists > 0) THEN
		return_node_list = next_node_list;
		next_node_list ++;
		free_node_lists --;
		return_node_list->next = NULL;
		return(return_node_list);
	ELSE
		next_node_list = malloc(BLOCKS_TO_PRELOAD * sizeof(node_list));
		IF (next_node_list == NULL) THEN
			fprintf(stderr, "SRD_Growth[%d]: Out of memory\n", pid);
			EXIT;
		FI
		free_node_lists = BLOCKS_TO_PRELOAD;
		return(new_node_list());
	FI
END

node *new_node() /* which is linked into first_nl --- last_nl  linked list */
BEGIN
	node *return_node;

	return_node = new_node_element();
	IF (last_nl == NULL) THEN
		last_nl = new_node_list();
		first_nl = last_nl;
		last_nl -> entry = return_node;
		return return_node;
	ELSE
		last_nl -> next = new_node_list();
		last_nl = last_nl -> next;
		last_nl -> entry = return_node;
		return return_node;
	FI
END


coeff_list *new_coeff_list()
BEGIN
	coeff_list *return_coeff_list;
	IF (free_coeff_lists > 0) THEN
		return_coeff_list = next_coeff_list;
		next_coeff_list ++;
		free_coeff_lists --;
		return_coeff_list->next = NULL;
		return(return_coeff_list);
	ELSE
		next_coeff_list = malloc(BLOCKS_TO_PRELOAD * sizeof(coeff_list));
		IF (next_coeff_list == NULL) THEN
			error_code = OUT_OF_MEMORY;
			fprintf(stderr, "SRD_Growth[%d]: Out of memory\n", pid);
			EXIT;
		FI
		free_coeff_lists = BLOCKS_TO_PRELOAD;
		return(new_coeff_list());  /** a recursion **/
	FI
END
