#include <stdio.h>
#include <time.h>
#include "real.h"

#define TRUE 1
#define FALSE 0

#define PI 3.1415926535897932384626433

GLOBAL  int		pid;
GLOBAL  time_t		timechecked;

GLOBAL  struct node_s		*initial_node_array;
GLOBAL 	node_list 	*first_nl, *last_nl;

GLOBAL	short		user_stops_exe;

GLOBAL	long 		iterations;
GLOBAL	long 		dumpidcount;
GLOBAL  int		number_of_nodes;
GLOBAL	int		random_seed;
GLOBAL	int		pickuprun;

GLOBAL	double		initA, B,
			a,b,c,d,
                        Kp,Su,Kd,Xth;
GLOBAL	double		X_diffuse, Y_diffuse, A_diffuse;

GLOBAL  double 		lambda;
GLOBAL  double		delta_t, deltat, radius, t0;
GLOBAL  double		time_start;
GLOBAL  double            max_initial_length_squared;
GLOBAL  double	        length_tolerance;


GLOBAL  double		area_growth_factor;
GLOBAL  double		max_X_value, max_Y_value, max_A_value,
			min_X_value, min_Y_value, min_A_value, total_X,
			total_relative_X, max_delta_X, min_X_value;

GLOBAL  double		max_abs_error;
GLOBAL  double		smoothing_factor;

GLOBAL int 		pic_freq;

GLOBAL  double		growth_step_rate, area_growth_tolerance;

GLOBAL  double		area_growth_factor, vector_growth_factor;

GLOBAL  double		growth_vector_length;

GLOBAL double radius;

GLOBAL  double		endtime;
GLOBAL  double		vector_growth_factor;
GLOBAL  double 		Xthreshold;
GLOBAL  int		moves_per_growth_step;
GLOBAL  int		growth_frequency;
GLOBAL  int 		refinement_frequency;
GLOBAL  int             growth_onset;

GLOBAL  double		k1, k2, k3, k4;
GLOBAL  double		kcr, k1prime, k4prime;
GLOBAL  short		turing_conditions_satisfied;


#define L_strings 200

GLOBAL char old_result_dir[L_strings];
GLOBAL char new_result_dir[L_strings];
GLOBAL char *exestopmsg;
GLOBAL char undumpfilename[L_strings];

/* General Macros */

#define SQUARE(NUMBER)  	( (NUMBER) * (NUMBER) )
#define MAX(A,B)		( ((A) > (B)) ? (A) : (B) )
#define ABS(NUMBER) 		( ( (NUMBER) > 0.0 ) ? (NUMBER) : (-(NUMBER)) )

#define EXIT  exit(1)

/* Error codes: */

GLOBAL short error_code;

#define FIND_NEIGHBOUR_TRIANGLES0	1
#define FIND_NEIGHBOUR_TRIANGLES1	2
#define FIND_NEIGHBOUR_TRIANGLES2	3
#define OUT_OF_MEMORY			4
#define NEIGHBOUR_LISTS_CONSISTENCY	5
