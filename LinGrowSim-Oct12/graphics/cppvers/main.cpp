#include <iostream>
using namespace std;

#include "BM.h"
#include "types.h"
#include "globals.h"
#include "display.h"
//#include "dump.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
// called in globals.h, necessary there. #include "types.h"

//extern node_list *first_nl;


void initialize_global_variables()
BEGIN
node *initial_node_array;

	initial_node_array = NULL; 
END

int main(int argc, char** argv)
BEGIN


REAL endtime;
extern char dumpdir[DUMPDIRSIZE];
extern int picturecounter;
int pic_freq;
short user_stops_exe, error_code;


   IF (argc<2) THEN
      sprintf(dumpdir, ".");
   ELSE
     strncpy(dumpdir, argv[1], 80);
     picturecounter = atoi(argv[2]);

   FI
	user_stops_exe = FALSE; 

	error_code = FALSE; 


      init_display(argc, argv);

     return 0;
END


