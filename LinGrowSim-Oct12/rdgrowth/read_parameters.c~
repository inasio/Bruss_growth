#include <time.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "BM.h"
#include "real.h"
#include "types.h"
#define GLOBAL extern
#include "globals.h"


char triangulation_filename[L_strings];


/*
 *
Function
	read_the_real_parameter

Function read_the_real_parameter takes a string argument and a pointer to a REAL.
If the string is the name of an environment variable then the REAL pointed
to is set to the value of the environment variable.

If no such environment variable is defined, an error message is written.

Returns:
	TRUE if an environment variable of the name specified is defined

	FALSE otherwise.
 *
 */
short read_the_real_parameter(char *varname, REAL *parameter)
BEGIN
	char *env_string;
 	IF ((env_string = getenv(varname)) != NULL) THEN
                *parameter = atof(env_string);
		return(TRUE);
        ELSE
		fprintf(stderr, "setenv %s ??!\n", varname);
		return(FALSE);
        FI
END

/*
 *
Function
	read_the_int_parameter

Function read_the_int_parameter takes a string argument and a pointer to a int.
If the string is the name of an environment variable then the int pointed
to is set to the value of the environment variable.

If no such environment variable is defined, an error message is written.

Returns:
	TRUE if an environment variable of the name specified is defined

	FALSE otherwise.
 *
 */
short read_the_int_parameter(char *varname, int *parameter)
BEGIN
	char *env_string;
 	IF ((env_string = getenv(varname)) != NULL) THEN
                *parameter = atoi(env_string);
		return(TRUE);
        ELSE
		fprintf(stderr, "setenv %s ??!\n", varname);
		return(FALSE);
        FI
END

/*
 *
Function
	read_the_string_parameter

Function read_the_string_parameter takes a string argument and a pointer to a string.
If the string is the name of an environment variable then the string pointed
to is copied from the value of the environment variable.

If no such environment variable is defined, an error message is written.

Returns:
	TRUE if an environment variable of the name specified is defined

	FALSE otherwise.
 *
 */
short read_the_string_parameter(char *varname, char *parameter, int maxlength)
BEGIN
	char *env_string;
 	IF ((env_string = getenv(varname)) != NULL) THEN
		strncpy(parameter, env_string, maxlength);
		return(TRUE);
        ELSE
		fprintf(stderr, "setenv  %s ??!\n", varname);
		return(FALSE);
        FI
END

/*
 *
Function
	read_parameters()

Function read_parameters reads all parameters via the functions defined above
according to values of environment variables.

Returns
	TRUE if all parameters could be read.
	FALSE otherwise.

 *
 */
short read_parameters()
BEGIN

       fprintf(stderr,"in read_parameters\n");

	return(
		read_the_real_parameter("SRD_Dx", &X_diffuse) &
		read_the_real_parameter("SRD_Dy", &Y_diffuse) &
		read_the_real_parameter("SRD_Da", &A_diffuse) &
		read_the_real_parameter("SRD_a", &a) &
		read_the_real_parameter("SRD_b", &b) &
		read_the_real_parameter("SRD_c", &c) &
		read_the_real_parameter("SRD_d", &d) &
		read_the_real_parameter("SRD_initA", &initA) &
		read_the_real_parameter("SRD_B", &B) &
		read_the_real_parameter("SRD_Kp", &Kp) &
		read_the_real_parameter("SRD_Su", &Su) &
		read_the_real_parameter("SRD_Kd", &Kd) &
		read_the_real_parameter("SRD_Xth", &Xth) &
		read_the_real_parameter("SRD_radius", &radius) &
		read_the_real_parameter("SRD_delta_t", &delta_t) &
		read_the_real_parameter("SRD_endtime", &endtime) &
		read_the_int_parameter("SRD_pic_freq", &pic_freq) &
		read_the_real_parameter("SRD_Area_Growth", &area_growth_factor) &
		read_the_int_parameter("SRD_moves_per_growth_step", &moves_per_growth_step) &
		read_the_int_parameter("SRD_growth_frequency", &growth_frequency) &
		read_the_int_parameter("SRD_growth_onset", &growth_onset) &
		read_the_real_parameter("SRD_length_tolerance", &length_tolerance) &
		read_the_int_parameter("SRD_random_seed", &random_seed) &
		read_the_string_parameter("SRD_triangulation",
					triangulation_filename, L_strings) &
		read_the_string_parameter("SRD_old_result_dir",
					old_result_dir, L_strings) &
		read_the_string_parameter("SRD_new_result_dir",
					new_result_dir, L_strings) &
		read_the_int_parameter("SRD_refinement_freq", &refinement_frequency) &
		read_the_int_parameter("SRD_pickuprun", &pickuprun) &

		read_the_string_parameter("SRD_undumpfilename",
					undumpfilename, L_strings)
 	);
END

/*
 *
Function
	produce_record()

Function produce_record() writes a record of the parameters specifying
the current computation to the file named HOW in the directory
specified by the string result_dir (a parameter read in
read_parameters()).

It also writes the name of the host on which the program runs and all
command line arguments including the program name to that file,
together with a time stamp.

Arguments:
argc and argv  - as for main (in order to be able to copy the command line).

Returns
	void
 *
 */

void save_parameters(int argc, char *argv[])
BEGIN
	FILE *f;
	int i;
	char filename[L_strings];
	char hostname[L_strings];
	time_t present_time;

	time(&present_time);

	/*
	 *
	Open file to write to.
  	 *
	 */
      if(pickuprun==FALSE)
      {
	sprintf(filename, "%s/HOW", old_result_dir);
      }
      else
      {
	sprintf(filename, "%s/HOW", new_result_dir);
      }
	f = fopen(filename, "w");
	IF (f == NULL) THEN
		fprintf(stderr, "Unable to append to HOW - file\n");
		EXIT;
	FI

	/*
 	 *
	Write all values of interest.
	 *
	 */
	fprintf(f,"setenv SRD_Dx  %g\n", X_diffuse);
	fprintf(f,"setenv SRD_Dy  %g\n", Y_diffuse);
	fprintf(f,"setenv SRD_Da  %g\n", A_diffuse);
	fprintf(f,"setenv SRD_a  %g\n", a);
	fprintf(f,"setenv SRD_b  %g\n", b);
	fprintf(f,"setenv SRD_c  %g\n", c);
	fprintf(f,"setenv SRD_d  %g\n", d);
	fprintf(f,"setenv SRD_initA  %g\n", initA);
	fprintf(f,"setenv SRD_B  %g\n", B);
	fprintf(f,"setenv SRD_Kp  %g\n", Kp);
	fprintf(f,"setenv SRD_Su  %g\n", Su);
	fprintf(f,"setenv SRD_Kd  %g\n", Kd);
	fprintf(f,"setenv SRD_radius  %g\n", radius);
	fprintf(f,"setenv SRD_delta_t  %g\n", delta_t);
	fprintf(f,"setenv SRD_endtime  %g\n", endtime);
	fprintf(f,"setenv SRD_pic_freq  %d\n", pic_freq);

	fprintf(f, "setenv SRD_Area_Growth %g\n", area_growth_factor);
	fprintf(f, "setenv SRD_moves_per_growth_step %d\n", moves_per_growth_step);
	fprintf(f, "setenv SRD_growth_frequency %d\n", growth_frequency);
	fprintf(f, "setenv SRD_length_tolerance %g\n", length_tolerance);
	fprintf(f, "setenv SRD_random_seed %d\n", random_seed);
	fprintf(f,"setenv SRD_triangulation  %s\n",
				triangulation_filename);
/*	fprintf(f,"setenv SRD_result_dir  %s\n", result_dir); */

	gethostname(hostname, 80);

	fprintf(f, "# Run on host '%s'\n", hostname);
	fprintf(f, "# on %s\n",(char *)(ctime(&present_time)));
	fprintf(f, "# with command\n# ");

	FOR (i=0; i<argc; i++) DO
		fprintf(f, "%s ", argv[i]);
		OD
	fprintf(f, "\n");
	fclose(f);
END
