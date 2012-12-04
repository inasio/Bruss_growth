#include "BM.h"
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#define GLOBAL extern
#include "globals.h"
#include "memory.h"
#include "triangle.h"


extern char triangulation_filename[80];
double initial_radius, center_height;

double lift(double x, double y)
BEGIN
	double rsquared = x*x + y*y;

	IF (rsquared > radius*radius) THEN
		return(0.0);
	ELSE	
		return(sqrt(radius*radius - rsquared));
	FI
END

       
short initialize_surface()
BEGIN
	FILE *f,*save;
	long i,j,k;
	int number_of_fixed_nodes;


	node_list *running_node;


    if(pickuprun == FALSE)
    {

	f = fopen(triangulation_filename, "r");


	IF (f == NULL) THEN
		fprintf(stderr, "SRD_Growth: Could not open `%s` to read initial triangulation!\n",
		triangulation_filename);
		return(FALSE);
	FI

	fscanf(f, "%d", &number_of_nodes);
	fscanf(f, "%d\n", &number_of_fixed_nodes);
	{
	int degree_which_is_not_needed; /* but in triangulation files */
	fscanf(f, "%d\n", &degree_which_is_not_needed);
	}


	initial_node_array = malloc (number_of_nodes * sizeof(struct node_s));


	IF (initial_node_array == NULL) THEN
		error_code = OUT_OF_MEMORY;
		fprintf(stderr, "\nSRD_Growth: Out of Memory\n");
	   	return(OUT_OF_MEMORY);
	FI



	FOR (i=0; i<number_of_nodes; i++) DO
	        float x_coor, y_coor;
		double rxcoor, rycoor;
		
		double rr;
		double correction_factor;

		fscanf(f, "%f", &x_coor);
		fscanf(f, "%f", &y_coor);

		rr = sqrt(x_coor*x_coor+y_coor*y_coor);
		IF (rr > 0) THEN
			correction_factor = cos(M_PI*0.5*(1-rr))/rr;
		ELSE
			correction_factor = 0.0;
		FI

		rxcoor = correction_factor* radius * (double) x_coor;
		rycoor = correction_factor* radius * (double) y_coor;
		initial_node_array[i].x = rxcoor;
		initial_node_array[i].y = rycoor;
		initial_node_array[i].z = lift(rxcoor, rycoor);

		initial_node_array[i].deltaX = 0.0;
		initial_node_array[i].deltaY = 0.0;
		initial_node_array[i].deltaA = 0.0;
		initial_node_array[i].fixed = FALSE;
		initial_node_array[i].conc_fixed = FALSE;


#ifdef 	IDENTIFY_NODES_AND_TRIANGLES
		initial_node_array[i].id = i;
#endif

	OD

        save = fopen("./initneighs.dat", "w"); 





	first_nl = new_node_list();
	running_node = first_nl;
	running_node -> entry = &(initial_node_array[0]);
	FOR (i=1; i<number_of_nodes; i++) DO
		running_node -> next = new_node_list();
		running_node = running_node -> next;
		running_node -> entry = &(initial_node_array[i]);
/*             fprintf(save,"%d,%f,%f,%f\n",running_node->entry->id, running_node->entry->x,running_node->entry->y, running_node->entry->z);*/
	OD

	running_node -> next = NULL;
	last_nl = running_node;

	FOR (i=0; i<number_of_fixed_nodes; i++)	DO
		initial_node_array[i].fixed = TRUE;
		initial_node_array[i].conc_fixed = TRUE;
	OD


	FOR (i=0; i<number_of_nodes; i++) DO
		int neighbour, dd;


		fscanf(f, "%d", &dd);
		initial_node_array[i].degree = dd;


           fprintf(save,"%ld,%f,%f,%f\n",i,initial_node_array[i].x,initial_node_array[i].y,initial_node_array[i].z);


		fscanf(f, "%d", &neighbour);
		initial_node_array[i].nl = new_node_list();
		running_node = initial_node_array[i].nl;
		running_node -> entry = &(initial_node_array[neighbour]);
		FOR (j=1; j<initial_node_array[i].degree; j++) DO
			fscanf(f, "%d", &neighbour);
			IF (neighbour != -1) THEN

				running_node -> next = new_node_list();
				running_node = running_node -> next;
				running_node -> entry = &(initial_node_array[neighbour]);
			FI

			running_node -> next = NULL;
		OD
	OD
	fclose(save);


		fclose(f); /* triangulation filename */

    }   /* closing new (t=0) run clause */

/* Following clause needed for new runs and old runs */

     printf("init_surface: number of nodes is %d\n", number_of_nodes);
#ifdef 	IDENTIFY_NODES_AND_TRIANGLES
		set_node_id(number_of_nodes);
#endif

	running_node = first_nl;
	WHILE(running_node != NULL) DO
		node *n = running_node -> entry;
	        coeff_list *cl_Lapl, *cl_Area, *cl_MatrixX, *cl_MatrixY, *cl_MatrixA;
		int i;
             if(pickuprun==0)
             {
               if(n->fixed==FALSE) n->conc_fixed=FALSE;
	     }

        	n->Lapl_coeff_l = new_coeff_list();
		n->Area_coeff_l = new_coeff_list();
		n->Matrix_coeff_l_X = new_coeff_list();
		n->Matrix_coeff_l_Y = new_coeff_list();
		n->Matrix_coeff_l_A = new_coeff_list();

		cl_Lapl = n->Lapl_coeff_l;
		cl_Area = n->Area_coeff_l;
		cl_MatrixX = n->Matrix_coeff_l_X;
		cl_MatrixY = n->Matrix_coeff_l_Y;
		cl_MatrixA = n->Matrix_coeff_l_A;

		FOR (i=1; i< n -> degree; i++) DO
			cl_Lapl->next = new_coeff_list();
			cl_Lapl = cl_Lapl->next;
			cl_Area->next = new_coeff_list();
			cl_Area = cl_Area->next;
			cl_MatrixX->next = new_coeff_list();
			cl_MatrixX = cl_MatrixX->next;
			cl_MatrixY->next = new_coeff_list();
			cl_MatrixY = cl_MatrixY->next;
			cl_MatrixA->next = new_coeff_list();
			cl_MatrixA = cl_MatrixA->next;
		OD

		running_node = running_node -> next;
	OD

	return(TRUE);

END
