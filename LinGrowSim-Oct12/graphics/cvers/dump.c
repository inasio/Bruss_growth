#include "BM.h"
#include "dump.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h> 
#include <time.h>
#include <math.h>
/*#include "types.h"*/

/*#define GLOBAL extern */

#include "globals.h"

/*#undef GLOBAL */

#include "memory.h"


/*GLOBAL int error_code;  */

#define FIND_NEIGHBOUR_TRIANGLES0	1
#define FIND_NEIGHBOUR_TRIANGLES1	2
#define FIND_NEIGHBOUR_TRIANGLES2	3
#define OUT_OF_MEMORY			4
#define NEIGHBOUR_LISTS_CONSISTENCY	5 


#define WRITE(name, file) fwrite(&(name), sizeof(name), 1, f)



#define READ(name, file)  fread(&(name), sizeof(name), 1, f)

short undump(char *filename)
BEGIN

FILE *f, *save;
int length, i;
int neighbour;

node_list *running_nl;
node_list *nl;
node  *running_node;

	fprintf(stderr, "Displayer: Attempting to open %s \n", filename);
	f = fopen(filename, "r");
	IF (f==NULL) THEN
		printf(" ... unsuccessful.\n");
		return(FALSE);
	ELSE
		printf(" ... ");
	FI

	READ(a, f);	READ(initA, f);
	READ(b, f);	READ(B, f);
	READ(c, f);
	READ(d, f);

        READ(X_diffuse, f); READ(Y_diffuse, f); READ(A_diffuse, f);
	READ(iterations, f);
        READ(dumpidcount, f);
	READ(delta_t, f);
        READ(max_initial_length_squared, f);

	READ(length, f);
	FOR (i=0; i<length; i++) DO
		READ(triangulation_filename[i], f);
	OD
	triangulation_filename[length] = '\0';

	READ(number_of_nodes, f);
/*        printf("undump: number_of_nodes is %d\n",number_of_nodes); */

/*	IF (initial_node_array != NULL) THEN
         	free(initial_node_array);
		FI */

      initial_node_array = malloc (number_of_nodes * sizeof(struct node_s)); 

	IF (initial_node_array == NULL) THEN
		error_code = OUT_OF_MEMORY;
		fprintf(stderr, "\nBrusselation: Out of Memory\n");
	   	EXIT;
	FI

	save = fopen("./initpos.dat","w");

	FOR (i=0; i<number_of_nodes; i++) DO
	        REAL x, y, z, X_val, Y_val, A_val;
		node *n = &(initial_node_array[i]);
		short j;

		READ(n->degree, f);
		READ(n->fixed, f);
                READ(n->conc_fixed, f);
                READ(n->id, f);
		READ(x, f); 
                READ(y, f); 
                READ(z, f);
		n->x = x;
                n->y = y; 
                n->z = z;
		READ(X_val, f); 
                READ(Y_val, f);
                READ(A_val, f);
		n->X = X_val; 
                n->Y = Y_val;
                n->A = A_val;
/*		n->id = i; */
            fprintf(save, "%d, %f, %f, %f, %f, %f, %d\n",n->id, n->x, n->y, n->z, n->X, n->Y,n->degree); 
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
		OD 
	OD

	fclose(save);
		/*       		printf("id is %d; x is %f\n",initial_node_array[10].id, initial_node_array[10].x);
					printf("a is %f;  b is %f\n", a, b); */

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
	return(TRUE);
END	
