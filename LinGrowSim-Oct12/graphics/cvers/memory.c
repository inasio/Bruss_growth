#include "BM.h"

/*#include "types.h" */
#include <malloc.h>
#include <stdio.h>


#include "globals.h" 

#define BLOCKS_TO_PRELOAD	4048

#ifdef USE_TRIANGLES
int free_triangles      = 0,
    free_triangle_lists = 0;
#endif


#define FIND_NEIGHBOUR_TRIANGLES0	1
#define FIND_NEIGHBOUR_TRIANGLES1	2
#define FIND_NEIGHBOUR_TRIANGLES2	3
#define OUT_OF_MEMORY			4
#define NEIGHBOUR_LISTS_CONSISTENCY	5

int free_nodes          = 0,
    free_node_lists     = 0;


#ifdef IDENTIFY_NODES_AND_TRIANGLES
int   node_id = 0;
#endif
    
node 	 	*next_node;
node_list 	*next_node_list;


void set_node_id(int v)
BEGIN
	node_id = v;
END

node *new_node()
BEGIN
	node *return_node;


	IF (free_nodes > 0) THEN
		return_node = next_node;
		next_node ++;
		free_nodes --;

		return_node->fixed=FALSE;


#ifdef IDENTIFY_NODES_AND_TRIANGLES
		return_node->id = node_id++;
#endif
		return_node -> nl = NULL;
		return(return_node);
	ELSE
		next_node = malloc(BLOCKS_TO_PRELOAD * sizeof(node));
		IF (next_node == NULL) THEN
				error_code = OUT_OF_MEMORY; 
		FI
		free_nodes = BLOCKS_TO_PRELOAD;
		return(new_node());   /** a recursion! **/
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
		free_node_lists = BLOCKS_TO_PRELOAD;
		return(new_node_list());
	FI
END


