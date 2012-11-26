#include "BM.h"
#include "globals.h"
#include "display.h"
#include "dump.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>



void initialize_global_variables()
BEGIN
	initial_node_array = NULL; 
END

int main(int argc, char** argv)
BEGIN

/*node_list *nl;*/

REAL endtime;
int pic_freq;
/*extern int picturecounter; */

   IF (argc<2) THEN
      sprintf(dumpdir, ".");
   ELSE
     strncpy(dumpdir, argv[1], 80);

   picturecounter = atoi(argv[2]);

   FI
	user_stops_exe = FALSE; 

	error_code = FALSE; 

/*	pid = getpid(); */


/*	fill_triangles = TRUE;	*/
/*	undump(filename); */
        
/*	nl = first_nl;

	WHILE (nl != NULL) DO
		node *n = nl->entry;

                printf("id %d; x %f; y %f; z %f\n",n->id, n->x, n->y, n->z);
		nl = nl->next;
		OD */

/*       newdisplay(argc, argv); */
   /*newdisplay.c function */

      init_display(argc, argv);
/*      pid = getpid(); */
/*      display_grid(); */

     return 0;
END


