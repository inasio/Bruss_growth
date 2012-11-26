#include <stdio.h>
#include <time.h>
#include "real.h" 
#include "types.h" 

#define TRUE 1
#define FALSE 0

#define PI 3.1415926535897932384626433

#define DUMPDIRSIZE 200
char dumpdir[DUMPDIRSIZE]; 
char		triangulation_filename[80];
char            undumpfilename[80];
int		pid;
time_t		timechecked;

struct node_s	  *initial_node_array;
node_list *first_nl, *last_nl;

#ifdef USE_TRIANGLES
struct triangle_s	*initial_triangle_array;
triangle_list 	*first_tl, *last_tl;
#endif


short		user_stops_exe;

long 		iterations;
long            dumpidcount;
int		random_seed, number_of_nodes;

REAL		initA, B, a, b, c, d;
REAL 		lambda;
REAL		delta_t, t0;
REAL		time_start;
REAL             max_initial_length_squared;
REAL	        length_tolerance;
float            common_rgbmap[256][3];

REAL		X_diffuse, Y_diffuse, A_diffuse;
REAL		area_growth_factor;
float           comp_time;


REAL		max_X_value, max_Y_value, max_A_value, 
			min_X_value, min_Y_value, min_A_value, total_X, 
			total_relative_X, max_delta_X;
			
REAL		max_abs_error;
REAL		smoothing_factor;

#ifdef USE_TRIANGLES
REAL 		max_triangle_area, max_side_of_triangle;
#endif

/*#ifdef SURFACE_GROWS
REAL		growth_step_rate, area_growth_tolerance;

REAL		area_growth_factor, vector_growth_factor;

REAL		growth_vector_length;
#endif */


REAL		endtime;
REAL		growth_period;
REAL		vector_growth_factor;
REAL		k1, k2, k3, k4;
REAL		kcr, k1prime, k4prime;
short		turing_conditions_satisfied;




/* General Macros */

#define SQUARE(NUMBER)  	( (NUMBER) * (NUMBER) )
#define MAX(A,B)		( ((A) > (B)) ? (A) : (B) )
#define ABS(NUMBER) 		( ( (NUMBER) > 0.0 ) ? (NUMBER) : (-(NUMBER)) )

#define EXIT  exit(1)

//#define no_DISPLAYING

/* Debugging stuff */


//#define DEBUGGING
//#define DEBUGINIT debug_counter = 1;
//#define DEBUGPRINT printf("Debugged to %d\n", debug_counter++); fflush(stdout);
//#define DEBUGMARK(x)  printf("MARK %d\n", x); fflush(stdout);
//#define DEBUGSTRING(s) printf(s); printf("\n"); fflush(stdout);

//int 		debug_counter;

/* Tracing with file "tracefile"  */
//FILE *tracefile;

/*#define TFN	"/usr/people/stephan/3dBrusselator/tracefile"
#define STARTTRACE(a) 	tracefile = fopen(TFN,"w");	\
		IF (tracefile != NULL) THEN		\
			fprintf(tracefile, a); 		\
			fclose(tracefile);		\
		FI

#define OPENTRACEFILE   
#define OLDOPENTRACEFILE tracefile = fopen(TFN, "w");
#define TRACE(T)        tracefile = fopen(TFN, "a");	\
			fprintf(tracefile, T);		\
			fclose(tracefile);
			#define CLOSETRACEFILE fclose(tracefile); */


/* Error codes */

short error_code; 

#define FIND_NEIGHBOUR_TRIANGLES0	1
#define FIND_NEIGHB
