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

// #define WRITE(name, file) fwrite(&(name), sizeof(name), 1, f)

#define dumpid(NODE) (* ( (int *)(& (NODE)->previous_X)))

/* int dumpidcount = 0; */
void dump()
{
	FILE *f;
	int nncount; /* to count the number of nodes */
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

	if (f == NULL) {
		fprintf(stderr, "... failed\n");
		return;
	}
	
	else {
		fprintf(stderr, "\n");
	}
	fprintf(f, "%f\n", a);	
	fprintf(f, "%f\n", initA);
	fprintf(f, "%f\n", b);
	fprintf(f, "%f\n", B);
	fprintf(f, "%f\n", c);
	fprintf(f, "%f\n", d);

        fprintf(f, "%f\n", X_diffuse);
	fprintf(f, "%f\n", Y_diffuse);
	fprintf(f, "%f\n", A_diffuse);	
	fprintf(f, "%d\n", iterations);
	fprintf(f, "%d\n", dumpidcount);
	fprintf(f, "%f\n", delta_t);
	fprintf(f, "%f\n", max_initial_length_squared);
	
// 	length = strlen(triangulation_filename);
// 	fprintf(f, "%d\n", length);
// 	FOR (i=0; i<length; i++) DO
// 		WRITE(triangulation_filename[i], f);
// 	OD

	nl  = first_nl;
	nncount = 0;
	while (nl != NULL) {
		node *n= nl->entry;
		dumpid(n) = nncount;
		nncount++;
/*                if(n->conc_fixed = TRUE) concfixedcount++; */
		nl = nl -> next;
	}

/* this gives the number of nodes! */
	number_of_nodes = nncount;
	fprintf(f, "%d\n", number_of_nodes);

	nl = first_nl;
        swcount = 0;
	while (nl != NULL) {
		node *n = nl->entry;
		node_list *nbl;
		double x, y, z, X_val, Y_val, A_val;

		fprintf(f, "%d\n", n->degree);
		fprintf(f, "%d\n", n->fixed);
		fprintf(f, "%d\n", n->conc_fixed);
		fprintf(f, "%d\n", n->id);
		x = n->x; y = n->y; z = n->z;
		fprintf(f, "%f\n", x);
		fprintf(f, "%f\n", y);
		fprintf(f, "%f\n", z);
                X_val = n->X; Y_val = n->Y; A_val = n->A;
		fprintf(f, "%f\n", X_val);
		fprintf(f, "%f\n", Y_val);
		fprintf(f, "%f\n", A_val);
		nbl = n->nl;
		WHILE(nbl!=NULL) {
			node *nb = nbl->entry;
			fprintf(f, "%d\n", dumpid(nb));
			nbl = nbl -> next;
		}
	        nl = nl->next;
	}
	fclose(f);
}


#define READ(name, file)  fread(&(name), sizeof(name), 1, f)

void undump(char *filename)
{
	FILE *f;
	int length, i, neighbour;
        char fullfilename[L_strings];
	node_list *running_nl;
	node_list *nl;
	node  *running_node;
        sprintf(fullfilename, "%s/%s",old_result_dir,filename);
	fprintf(stderr, "Attemtping to open %s", fullfilename);
	f = fopen(fullfilename, "r");
	if (f==NULL) {
		fprintf(stderr, " ... unsuccessful.\n");
		return;
	}
	else  {
		fprintf(stderr, " ... ");
	}

	/* NOTE - this overwrites the values in TEST, disable this
                  READ if these parameters need changing between 
                  initial and pick-up runs. 8/21/03. */
  
	fscanf(f, "%f\n", &a);	
	fscanf(f, "%f\n", &initA);
	fscanf(f, "%f\n", &b);
	fscanf(f, "%f\n", &B);
	fscanf(f, "%f\n", &c);
	fscanf(f, "%f\n", &d);

        fscanf(f, "%f\n", &X_diffuse);
	fscanf(f, "%f\n", &Y_diffuse);
	fscanf(f, "%f\n", &A_diffuse);
	fscanf(f, "%d\n", &iterations);
	fscanf(f, "%d\n", &dumpidcount); 

	fscanf(f, "%f\n", &delta_t);
	fscanf(f, "%f\n", &max_initial_length_squared);

// 	READ(length, f);
// 	FOR (i=0; i<length; i++) DO
// 		READ(triangulation_filename[i], f);
// 	OD
// 	triangulation_filename[length] = '\0';

	fscanf(f, "%d\n", &number_of_nodes);
	printf("undump: number_of_nodes is %d\n",number_of_nodes);
	initial_node_array = malloc (number_of_nodes * sizeof(struct node_s));

	if (initial_node_array == NULL) {
 		error_code = OUT_OF_MEMORY;
		fprintf(stderr, "\nSRD_Growth: Out of Memory\n");
	   	exit(1);
	}


	for (i=0; i<number_of_nodes; i++) {
		double x,y,z, X_val, Y_val, A_val;
		node *n = &(initial_node_array[i]);
		short j;

		fscanf(f, "%d\n", &n->degree);
		fscanf(f, "%d\n", &n->fixed);
		fscanf(f, "%d\n", &n->conc_fixed);
		fscanf(f, "%d\n", &n->id);
		fscanf(f, "%f\n", &x);
		fscanf(f, "%f\n", &y);
		fscanf(f, "%f\n", &z);
		n->x = x; n->y = y; n->z = z;
		fscanf(f, "%f\n", &X_val);
		fscanf(f, "%f\n", &Y_val);
		fscanf(f, "%f\n", &A_val);
                n->X = X_val; n->Y = Y_val; n->A = A_val;

		n->nl = new_node_list();
		running_nl = n->nl;
		fscanf(f, "%d\n", &neighbour);
		running_nl -> entry = &(initial_node_array[neighbour]);

		for (j=1; j<n->degree; j++) {
			fscanf(f, "%d\n", &neighbour);
			running_nl -> next = new_node_list();
			running_nl = running_nl -> next;
			running_nl -> entry = &(initial_node_array[neighbour]);
			running_nl -> next = NULL;
		}
	}

	first_nl = new_node_list();
	running_nl = first_nl;
	running_nl -> entry = &(initial_node_array[0]);
	for (i=1; i<number_of_nodes; i++) {
		running_nl -> next = new_node_list();
		running_nl = running_nl -> next;
		running_nl -> entry = &(initial_node_array[i]);
	}

	running_nl -> next = NULL;
	last_nl = running_nl;

	fclose(f);
	fprintf(stderr, "done.\n");
}
