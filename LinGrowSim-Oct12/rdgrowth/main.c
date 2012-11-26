#include "BM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include "types.h"
#include "setup_matrix.h"

#define GLOBAL
#include "globals.h"
#undef GLOBAL

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
#include "loop.h"

extern char triangulation_filename[80];


void setup_everything(int argc, char *argv[])
BEGIN

	user_stops_exe = FALSE;
	error_code = 0;
	pid = getpid();

      if (read_parameters() == FALSE)
      {
	   fprintf(stderr, "\nSRD_Growth: Error in reading parameters.\n");
	   EXIT;
      }
      else
      {
	save_parameters(argc, argv);

        if(pickuprun == FALSE)
        {
	  /*  fprintf(stderr, "setup everything 1\n"); */
	  IF (!initialize_surface()) THEN
	      fprintf(stderr, "\nSRD_Growth: Error in initializing surface\n");
		EXIT;
	  FI

	    /*        fprintf(stderr, "setup everything 2\n"); */

   	  time_start = 0.0;
	  pid = getpid();
	  timechecked = time(NULL);
	  init_distribution();


	  count_node_degrees();
	  find_maximal_initial_length();
          printf("max_init_length is %f\n", max_initial_length_squared);
          fprintf(stderr, "SRD_Growth[%d]: Setting up matrices...\n", pid);

	  initialize_imex_arrays();
	  setup_matrix_coefficients();
	  set_RK_matrix_coefficients(0);

	}


        else
        {
           undump(undumpfilename);
           iterations++;

	   /*        fprintf(stderr, "setup everything 1.1\n"); */
	  IF (!initialize_surface()) THEN
	     fprintf(stderr, "\nSRD_Growth: Error in initializing surface\n");
		EXIT;
	  FI

	    /*        fprintf(stderr, "setup everything 2.1\n"); */

	  pid = getpid();

 	   count_node_degrees();
   
          printf("max_init_length is %f\n", max_initial_length_squared);
        
	   fprintf(stderr, "SRD_Growth[%d]: Setting up matrices...\n", pid);

	   initialize_imex_arrays(); 
           setup_matrix_coefficients();
	   set_RK_matrix_coefficients(0); 
	}
      }

END

int main (int argc, char *argv[])
BEGIN
	int pic_freq;

/*        fprintf(stderr, "starting main, before setup everything\n"); */

	setup_everything(argc, argv);

	fprintf(stderr, "SRD_Growth[%d]: Running.\n", pid);

	if(pickuprun==FALSE)
        {
           iterations = 0;
           dumpidcount = 0;
	}
        else
        {
          if(dumpidcount>0) dumpidcount--;
	}

	computation_loop();

	/*
	 *
	Final dump.
	 *
 	 */
	dump();
	fprintf(stderr, "SRD_Growth[%d]: Done after %ld iterations.\n", pid, iterations);
END
