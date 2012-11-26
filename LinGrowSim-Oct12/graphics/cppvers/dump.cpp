#include "BM.h"
#include "types.h"
#include "globals.h"
#include "dump.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h> 
#include <time.h>
#include <math.h>

#include "memory.h"


/*GLOBAL int error_code;  */

#define FIND_NEIGHBOUR_TRIANGLES0	1
#define FIND_NEIGHBOUR_TRIANGLES1	2
#define FIND_NEIGHBOUR_TRIANGLES2	3
#define OUT_OF_MEMORY			4
#define NEIGHBOUR_LISTS_CONSISTENCY	5 


#define WRITE(name, file) fwrite(&(name), sizeof(name), 1, f)



#define READ(name, file)  fread(&(name), sizeof(name), 1, f)

REAL		A, B, a, b, c, d;
REAL		X_diffuse, Y_diffuse;
char		triangulation_filename[80];
int		number_of_nodes;
node	  *initial_node_array;
node_list *last_nl;


short undump(char *filename)
BEGIN

FILE *f, *save;
int length, i;
int neighbour;
node_list *running_nl;
node_list *nl;
node  *running_node;

	printf("Attempting to open %s\n", filename);
	f = fopen(filename, "r");
	IF (f==NULL) THEN
			printf(" ... unsuccessful.\n");
		return(0);
	ELSE
		printf(" ... ");
	FI

	READ(a, f);	READ(A, f);
	READ(b, f);	READ(B, f);
	READ(c, f);
	READ(d, f);

	READ(X_diffuse, f); READ(Y_diffuse, f);
	READ(iterations, f);
	READ(delta_t, f);

	READ(length, f);


	FOR (i=0; i<length; i++) DO
		READ(triangulation_filename[i], f);
	OD
	triangulation_filename[length] = '\0';

	READ(number_of_nodes, f);


	IF (initial_node_array != NULL) THEN
         	free(initial_node_array);
	FI
 

  initial_node_array = (node *)malloc(number_of_nodes * sizeof(node));

//      initial_node_array = new node [number_of_nodes]; 


	IF (initial_node_array == NULL) THEN
		error_code = OUT_OF_MEMORY;
		fprintf(stderr, "\nBrusselation: Out of Memory\n");
	   	EXIT;
	FI

	save = fopen("./initpos.dat","w");

	FOR (i=0; i<number_of_nodes; i++) DO
		REAL x, y, z, X_val, Y_val;
		node *n = &(initial_node_array[i]);
		short j;

		READ(n->degree, f);
		READ(n->fixed, f);
                READ(n->conc_fixed, f);
		READ(x, f); 
                READ(y, f); 
                READ(z, f);
		n->x = x;
                n->y = y; 
                n->z = z;
		READ(X_val, f); 
                READ(Y_val, f);
		n->X = X_val; 
                n->Y = Y_val;
		n->id = i;
            fprintf(save, "%d, %d\n",n->id, n->degree); 
		n->nl = new_node_list();
		running_nl = n->nl;
		READ(neighbour, f);
		running_nl -> entry = &(initial_node_array[neighbour]);

		FOR (j=1; j<n->degree; j++) DO
			READ(neighbour, f);
			running_nl -> next = new_node_list();
			running_nl = running_nl -> next;
			running_nl -> entry = &(initial_node_array[neighbour]);
			running_nl -> next = NULL;
                        fprintf(save, "%d,",neighbour);  
		OD 
	       fprintf(save, "\n");  
	OD

	fclose(save);

	first_nl = new_node_list();
	running_nl = first_nl;
	running_nl -> entry = &(initial_node_array[0]);
	FOR (i=1; i<number_of_nodes; i++) DO
		running_nl -> next = new_node_list();
		running_nl = running_nl -> next;
		running_nl -> entry = &(initial_node_array[i]);
	OD

	running_nl -> next = NULL;
	last_nl = running_nl;
	    	
	fclose(f);

	printf("done.\n");
	printf("[%s] Number of nodes: %d\n", filename,number_of_nodes);
	return(1);
END	
