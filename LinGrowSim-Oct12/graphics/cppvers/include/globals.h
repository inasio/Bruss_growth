#include <stdio.h>
#include <time.h>
#include "real.h" 
/*#include "types.h" */

#define _GLOBALS_ON_ 

#define TRUE 1
#define FALSE 0

#define PI 3.1415926535897932384626433

#define DUMPDIRSIZE 200
extern char dumpdir[DUMPDIRSIZE]; 
extern char		triangulation_filename[80];
extern char             dumpmsg[80], areamsg[80], nodesmsg[80], zmaxmsg[80] ;
extern char             msg[80], morphminmsg[80], morphmaxmsg[80];
extern char             jumpmsg[100], newmsg[100];
extern int		pid, jumpnum;
extern int              picturecounter;
extern time_t		timechecked;

extern node	  *initial_node_array;
extern node_list *first_nl, *last_nl;

#ifdef USE_TRIANGLES
struct triangle_s	*initial_triangle_array;
triangle_list 	*first_tl, *last_tl;
#endif


extern short		user_stops_exe;

extern long 		iterations;
extern int 		gizmocounter;
extern int              gizmowindname, gizmosubwindname;
extern int		number_of_nodes;
extern int		random_seed, number_of_nodes;

extern REAL		A, B, a, b, c, d;
extern REAL 		lambda;
extern REAL		delta_t, t0;
extern REAL		time_start;
extern REAL             max_initial_length_squared;
extern REAL	        length_tolerance, the_area, max_z;
extern float            common_rgbmap[256][3];

extern REAL		X_diffuse, Y_diffuse;

extern REAL		area_growth_factor;
extern float           comp_time;

/*extern REAL             max_distance; */

extern REAL		max_X_value, max_Y_value, max_A_value, 
			min_X_value, min_Y_value, min_A_value, total_X, 
			total_relative_X, max_delta_X;
			
extern REAL		max_abs_error;
extern REAL		smoothing_factor;

#ifdef USE_TRIANGLES
REAL 		max_triangle_area, max_side_of_triangle;
#endif

#ifdef SURFACE_GROWS
REAL		growth_step_rate, area_growth_tolerance;

REAL		area_growth_factor, vector_growth_factor;

REAL		growth_vector_length;
#endif


extern REAL		endtime;
extern REAL		growth_period;
extern REAL		vector_growth_factor;
extern REAL		k1, k2, k3, k4;
extern REAL		kcr, k1prime, k4prime;
extern short		turing_conditions_satisfied;




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

extern short error_code; 

#define FIND_NEIGHBOUR_TRIANGLES0	1
#define FIND_NEIGHB
