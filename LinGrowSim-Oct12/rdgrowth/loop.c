#include "BM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "types.h"
#include "setup_matrix.h"

#define GLOBAL extern
#include "globals.h"

#include "read_parameters.h"
#include "init_surface.h"
#include "init_distribution.h"
#include "dump.h"
#ifdef DISPLAYING
#include "display.h"
#include <gl/gl.h>
#endif

#include "reaction_diffusion.h"
#include "triangle.h"
#include "refine.h"

void computation_loop()
BEGIN
	time_t comp_start_time;
	long start_iterations, end_iteration;
	long refined;
	REAL growth_time;
        FILE *save;
        node_list *nl;
        int i;
	time(&comp_start_time);
	start_iterations = iterations;
	end_iteration = (long)(endtime / delta_t);

	user_stops_exe = FALSE;

	growth_time = 0.0;

	dump();


	WHILE( !user_stops_exe )
	DO
	     time_t present_time;
	     REAL min_to_go;


	     IF (iterations * delta_t > endtime) THEN
               printf("iterations * delta_t > endtime\n");
			break;
	     FI

             time(&present_time);

             fprintf(stderr, "Iteration %5ld, comp.time %4.5g; local time %s",
                       iterations, (REAL) iterations*delta_t,
                        (char *)(ctime(&present_time)+11));

		/*  Call Reaction-Diffusion every iteration */

                reaction_diffusion();
        	
         if(iterations % 10 == 0) add_noise(); /* added (laptop) 12/16/03 */



        if((iterations % growth_frequency == 0) && (iterations>growth_onset))
        {
	    growth();

	    if(refine_if_necessary()); 
	}


/* NOTE for below, conc_fixed flagging is turned off in r_d.c, 7/17/03 */
/* Tally method here updated from driftwood.iam, 12/16/03 */
        if(iterations % pic_freq ==0 && iterations != 0) 
        {
            int tally = 0;

            nl = first_nl;
            WHILE (nl != NULL) DO
	      node *n = nl -> entry;

 	      if(n->X < Xth) tally++; /* added (driftwood) 5/14/03 */

              nl = nl->next;
            OD

	    if(tally==0) user_stops_exe == TRUE; 

            exestopmsg = "all nodes below Xth";

            dump();
	   } 

	iterations ++;

	OD

        if(user_stops_exe == TRUE) 
        {

  	  FILE *f;
	  int i;
	  char filename[L_strings];
	  time_t present_time;

	  time(&present_time);

	/*
	 *
	Open file to write to.
  	 *
	 */
	if(pickuprun==FALSE)
        {
	   sprintf(filename, "%s/FINISH", old_result_dir);
	}
        else
	{
	   sprintf(filename, "%s/FINISH", new_result_dir);
	}       
	f = fopen(filename, "w");
	IF (f == NULL) THEN
		fprintf(stderr, "Unable to append to FINISH - file\n");
		EXIT;
	FI

	/*
 	 *
	Write all values of interest.
	 *
	 */
	fprintf(f, "Computation stopped on\n");
	fprintf(f, "# on %s\n",(char *)(ctime(&present_time)));
        fprintf(f, "because of\n");
        fprintf(f, "%s\n", exestopmsg);



	fclose(f);
	}
END


