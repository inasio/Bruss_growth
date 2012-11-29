#include "BM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "types.h"
#include "memory.h"
#define GLOBAL
#include "globals.h"
#undef GLOBAL


extern char triangulation_filename[80];

#define WRITE(name, file) fwrite(&(name), sizeof(name), 1, f)

#define dumpid(NODE) (* ( (int *)(& (NODE)->previous_X)))

/* int dumpidcount = 0; */
void dump()
BEGIN
	FILE *f;
	int nncount; /* to count the number of nodes */
/*        int concfixedcount; */
	int length, i;
	char filename[L_strings];
	node_list *nl;
        int swcount;

      if(pickuprun==FALSE)
      {
	sprintf(filename, "%s/dump%ld", old_result_dir, dumpidcount);
      }
      else
      {
	sprintf(filename, "%s/dump%ld", new_result_dir, dumpidcount);
      }    
	dumpidcount ++;
	fprintf(stderr, "SRD_Growth[%d]: Dumping to file '%s'.", pid,filename);
	f = fopen(filename, "w");

	IF (f == NULL) THEN
		fprintf(stderr, "... failed\n");
		return;
	ELSE
		fprintf(stderr, "\n");
	FI
	WRITE(a, f);	WRITE(initA, f);
	WRITE(b, f);	WRITE(B, f);
	WRITE(c, f);
	WRITE(d, f);

        WRITE(X_diffuse, f); WRITE(Y_diffuse, f); WRITE(A_diffuse, f);
	WRITE(iterations, f);
	WRITE(dumpidcount, f);
	WRITE(delta_t, f);
        WRITE(max_initial_length_squared, f);

	length = strlen(triangulation_filename);
	WRITE(length, f);
	FOR (i=0; i<length; i++) DO
		WRITE(triangulation_filename[i], f);
	OD

	nl  = first_nl;
	nncount = 0;
/*        concfixedcount = 0; */
	WHILE (nl != NULL) DO
		node *n= nl->entry;
		dumpid(n) = nncount;
		nncount++;
/*                if(n->conc_fixed = TRUE) concfixedcount++; */
		nl = nl -> next;
	OD

/* this gives the number of nodes! */
	number_of_nodes = nncount;
	WRITE(number_of_nodes, f);

	nl = first_nl;
        swcount = 0;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		node_list *nbl;
		REAL x, y, z, X_val, Y_val, A_val;

		WRITE(n->degree, f);
		WRITE(n->fixed, f);
		WRITE(n->conc_fixed, f);
                WRITE(n->id, f);
		x = n->x; y = n->y; z = n->z;
/*       printf("id %d; x %f; y %f; z %f\n", n->id, n->x, n->y, n->z); */
		WRITE(x, f);
		WRITE(y, f);
		WRITE(z, f);
                X_val = n->X; Y_val = n->Y; A_val = n->A;
		WRITE(X_val, f);
		WRITE(Y_val, f);
		WRITE(A_val, f);
		nbl = n->nl;
		WHILE(nbl!=NULL) DO
			node *nb = nbl->entry;
			WRITE(dumpid(nb), f);
			nbl = nbl -> next;
		OD
	        nl = nl->next;
	OD

	    fclose(f);

/*        if(number_of_nodes == concfixedcount) user_stops_exe = TRUE; */


END


#define READ(name, file)  fread(&(name), sizeof(name), 1, f)

void undump(char *filename)
BEGIN
	FILE *f;
	int length, i, neighbour;
        char fullfilename[L_strings];
	node_list *running_nl;
	node_list *nl;
	node  *running_node;
        sprintf(fullfilename, "%s/%s",old_result_dir,filename);
	fprintf(stderr, "Attemtping to open %s", fullfilename);
	f = fopen(fullfilename, "r");
	IF (f==NULL) THEN
		fprintf(stderr, " ... unsuccessful.\n");
		return;
	ELSE
		fprintf(stderr, " ... ");
	FI

	/* NOTE - this overwrites the values in TEST, disable this
                  READ if these parameters need changing between 
                  initial and pick-up runs. 8/21/03. */
  
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
       printf("undump: number_of_nodes is %d\n",number_of_nodes);
	initial_node_array = malloc (number_of_nodes * sizeof(struct node_s));

	IF (initial_node_array == NULL) THEN
 		error_code = OUT_OF_MEMORY;
		fprintf(stderr, "\nSRD_Growth: Out of Memory\n");
	   	EXIT;
	FI


	FOR (i=0; i<number_of_nodes; i++) DO
		REAL x,y,z, X_val, Y_val, A_val;
		node *n = &(initial_node_array[i]);
		short j;

		READ(n->degree, f);

/*           printf("degree is %d,", n->degree); */
		READ(n->fixed, f);
/*           printf("fixed is %d\n", n->fixed); */
		READ(n->conc_fixed, f);
                READ(n->id, f);
		READ(x,f); READ(y,f); READ(z,f);
		n->x = x; n->y = y; n->z = z;
                READ(X_val, f); READ(Y_val,f); READ(A_val, f);
                n->X = X_val; n->Y = Y_val; n->A = A_val;

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
	fprintf(stderr, "done.\n");
END	
