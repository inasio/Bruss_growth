#include "BM.h" 
//#include "real.h"
#include "dump.h"

#include <iostream>
using namespace std;

#include <stdio.h>  
#include <stdlib.h> 
//#include <malloc.h>
#include <cmath>
#include "types.h" 

//#ifndef _GLOBALS_ON_
#include "globals.h"
//#endif

#include "triangle.h"
#include "display.h"

#include "glow.h"
GLOW_NAMESPACE_USING
#include "dispbuttons.h"
#include <GL/glut.h>

//#include <cstdio>
//#include <algorithm>
//#include "glowViewTransform.h"
//#include "mandelgizmo.h"

#define ABS abs

//extern char dumpdir[DUMPDIRSIZE];
node_list *first_nl;
REAL		max_X_value, max_Y_value, max_A_value, 
			min_X_value, min_Y_value, min_A_value, total_X, 
			total_relative_X, max_delta_X;
int 		 pid;
int              picturecounter;
char dumpdir[DUMPDIRSIZE];
long 		iterations;
REAL		delta_t, t0;
int windowID;

unsigned long *window_pixels_buffer = NULL;

/*void set_scale_input(float);*/

short colors_valid = TRUE;

int printcounter = 0;

struct triangle_s {
  float v0[3], v1[3], v2[3];
  float X,Y;
  int NX[3], NY[3], NA[3];
};

#define WHITEONBLACK

short find_initial_scale = TRUE;
struct triangle_s *triangles = NULL;
int max_triangle;

extern int picturecounter;
REAL the_area;
int _mainWindowID;
REAL max_z;

long 	gid;
float 	rotate_x, rotate_y, rotate_z;  
short 	xcoor, ycoor, oldxcoor, oldycoor;
float 	scale_value, comp_time;

short window_is_visible = TRUE;
short   mode=1;
short show_grid = FALSE;
short active = TRUE;

int present_picture = 0;
int fill_triangles;

float common_rgbmap[256][3];
int print_rgbmap[256][3];

void init_rgb_maps()
BEGIN

/*float value; */

#define SET_RED(value)    common_rgbmap[i][0] = (value);
#define SET_GREEN(value)  common_rgbmap[i][1] = (value);
#define SET_BLUE(value)   common_rgbmap[i][2] = (value); 

	int i;
/*	common_rgbmap[0][0] = 1.0;
	common_rgbmap[0][1] = 0.0;
	common_rgbmap[0][2] = 1.0; */

/*	i=0;
	SET_RED(0); SET_GREEN(0.98); SET_BLUE(0); */

	FOR (i=0; i<64; i++) DO 
		SET_RED(0.0);		SET_GREEN(0.4);	  SET_BLUE(4.0*i/504.0);
	OD
	FOR (i=64; i<128; i++) DO
		SET_RED(4.0*(i-64.0)/504.0);	SET_GREEN(0.3);		SET_BLUE(0.8+((i-64)/320.0));
	OD
	FOR (i=128; i<192; i++) DO
		SET_RED(1.0);		SET_GREEN(0.3+((4.0*(i-128.0))/425.0)); 	SET_BLUE(0.3);
	OD
	FOR (i=192; i<256; i++) DO
		SET_RED(1.0);		
                SET_GREEN(0.9+((i-192)/640.0));	
                SET_BLUE(0.2+((4.0*(i-192.0))/319.0));
 	OD 
	       
	/*#define SET_RED(value)    print_rgbmap[i][0] = (value);
 #define SET_GREEN(value)  print_rgbmap[i][1] = (value);
 #define SET_BLUE(value)   print_rgbmap[i][2] = (value);
	FOR (i=0; i<256; i++) DO
		SET_RED(i); SET_GREEN(i); SET_BLUE(i);
	OD
	FOR (i=0; i<128; i++) DO 
		SET_RED(0);		SET_GREEN( 252 - 2*i );	SET_BLUE(2*i);
	OD
	FOR (i=128; i<256; i++) DO
		SET_RED(2*(i-128));	SET_GREEN(0);		SET_BLUE(252-2*(i-128));
		OD */
		

/* 	FOR (i=0; i<256; i++) DO
	 	SET_RED(0); 
                SET_GREEN(0); 
                SET_BLUE(i/255.0);
		OD */

END

void count_node_degrees()
BEGIN
	int max_degree = 0;
	int node_counter = 0;
	int dc;
	node_list *nl;
        node *n;

	max_X_value = -1000; min_X_value = 10000;
	max_Y_value = -1000; min_Y_value = 10000;
	max_A_value = -1000; min_A_value = 10000;
	max_z = 0.0;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node_list *nbl;
	        node_counter ++;
		n = nl -> entry;

		IF (max_X_value < n->X) THEN max_X_value = n->X; FI
		IF (max_Y_value < n->Y) THEN max_Y_value = n->Y; FI
		IF (max_A_value < n->A) THEN max_A_value = n->A; FI
		IF (min_X_value > n->X) THEN min_X_value = n->X; FI
		IF (min_Y_value > n->Y) THEN min_Y_value = n->Y; FI
		IF (min_A_value > n->A) THEN min_A_value = n->A; FI
		IF (n->z        > max_z) THEN max_z = n->z; FI

		dc = 0;
		nbl = n->nl;
		WHILE (nbl != NULL) DO    dc ++; nbl=nbl->next; OD
		
		IF (dc > max_degree) THEN max_degree = dc; FI
	
		nl = nl -> next;
	OD

	IF ( ABS(max_X_value - min_X_value)<1e-10) THEN
		max_X_value = 2*ABS(max_X_value) + 2;
	FI
	IF ( ABS(max_Y_value - min_Y_value)<1e-10) THEN
		max_Y_value = 2*ABS(max_Y_value) + 2;
	FI


	IF ( ABS(max_A_value - min_A_value)<1e-10) THEN
		max_A_value = 2*ABS(max_A_value) + 2;
	FI

	printf("MaxX=%f MinX=%f MaxY=%f MinY=%f\n",
		max_X_value,
		min_X_value,
		max_Y_value,
	       min_Y_value);
        printf("MaxA=%f MinA=%f\n", max_A_value, min_A_value);
        printf("Max z = %f\n", max_z);
END

void set_initial_scale()
BEGIN
        REAL max_distance = 0.0;
	REAL inter_node_distance = 0.0;
	node_list *nl;

//        max_distance = 0.0;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n=nl->entry;
		node_list *nb = n->nl;
		REAL x = n->x, y = n->y, z = n->z;
		REAL dist = sqrt(x*x+y*y+z*z);	
		IF (dist > max_distance) THEN
			max_distance = dist;
		FI;
		WHILE (nb != NULL) DO
			node *m = nb->entry;
			REAL xm = m->x, ym= m->y, zm= m->z;
			REAL d = sqrt((x-xm)*(x-xm)+(y-ym)*(y-ym)+(z-zm)*(z-zm));
			IF (d>inter_node_distance) THEN
				inter_node_distance = d;
			FI;
			nb = nb->next;
		OD
		nl = nl->next;
	OD
	scale_value = 0.93 / max_distance;
	printf("Internode-distance bounded by %f\n", inter_node_distance);
END
	
int triangle_count()
BEGIN
	node_list *nl;
	int tc;
	tc = 0; 
	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		node_list *nbl = n->nl;
		node *n1 = nbl->entry;
		WHILE (nbl->next != NULL) DO
			node *n1 = nbl->entry, *n2 = nbl->next->entry;
			IF (n->id < n1->id || n->id < n2->id) THEN
				tc++;
			FI
			nbl=nbl->next;
		OD
		IF (!n->fixed) THEN
			node *n2 = nbl->entry;
			IF (n->id < n1->id || n->id < n2->id) THEN
				tc++;
			FI
		FI
		nl = nl->next;
	OD
	return(tc);
END			
				
short use_users_ranges = FALSE;
REAL user_max_X = 0.0, user_min_X = 0.0, user_max_Y = 0.0, user_min_Y = 0.0;
REAL user_max_A = 0.0, user_min_A = 0.0;

void init_triangles()
BEGIN
	node_list *nl;
	int tc;
//	REAL the_area;
	int i;
	max_triangle = triangle_count();

       printf("triangle count is %d\n", max_triangle);

	IF (find_initial_scale) THEN
	/*extern void set_scale_input(float);*/
		set_initial_scale();	
		find_initial_scale = FALSE;
/*		set_scale_input(scale_value);*/
	FI		
	IF (triangles != NULL) THEN
		free(triangles);
	FI
	triangles = (struct triangle_s *) malloc(max_triangle * sizeof(*triangles));

	count_node_degrees(); /* to get max_X_value .. */
	nl = first_nl;
	tc = 0;
        the_area = 0.0;
/* DH, 1/3/03, looks like use_users_ranges could be used for text box input of any desired colour scale, rather than locking into max, min morphogen values. */
	IF (use_users_ranges) THEN
		IF (user_max_X > max_X_value) THEN max_X_value = user_max_X; FI
		IF (user_min_X < min_X_value) THEN min_X_value = user_min_X; FI
		IF (user_max_Y > max_Y_value) THEN max_Y_value = user_max_Y; FI
		IF (user_min_Y < min_Y_value) THEN min_Y_value = user_min_Y; FI
		IF (user_max_A > max_A_value) THEN max_A_value = user_max_A; FI
		IF (user_min_A < min_A_value) THEN min_A_value = user_min_A; FI
		max_X_value = user_max_X;
		max_Y_value = user_max_Y;
		min_X_value = user_min_X;
		min_Y_value = user_min_Y;
		min_A_value = user_min_A;
		min_A_value = user_min_A;
	FI

	WHILE (nl != NULL) DO
		node *n = nl->entry;
		node_list *nbl = n->nl;
		node *n1 = nbl->entry;
		WHILE (nbl->next != NULL) DO
			node *n1 = nbl->entry, *n2 = nbl->next->entry;
	           IF (n->id < n1->id || n->id < n2->id) THEN 
the_area += area_of_triangle(n,n1,n2);
			triangles[tc].v0[0] = n->x;triangles[tc].v0[1] = n->y;triangles[tc].v0[2]  =n->z;
			triangles[tc].v1[0] = n1->x;triangles[tc].v1[1] = n1->y;triangles[tc].v1[2] =n1->z;
			triangles[tc].v2[0] = n2->x;triangles[tc].v2[1] = n2->y;triangles[tc].v2[2] =n2->z;

			triangles[tc].NX[0] =  (int) (255.0 * (n->X - min_X_value) / (max_X_value - min_X_value));
			triangles[tc].NX[1] =  (int) (255.0 * (n1->X - min_X_value) / (max_X_value - min_X_value));
			triangles[tc].NX[2] =  (int) (255.0 * (n2->X - min_X_value) / (max_X_value - min_X_value));

			triangles[tc].NY[0] = (int) (255.0 * (n->Y - min_Y_value) / (max_Y_value - min_Y_value));
			triangles[tc].NY[1] = (int) (255.0 * (n1->Y - min_Y_value) / (max_Y_value - min_Y_value));
			triangles[tc].NY[2] = (int) (255.0 * (n2->Y - min_Y_value) / (max_Y_value - min_Y_value));

		triangles[tc].NA[0] = (int) (255.0 * (n->A - min_A_value) / (max_A_value - min_A_value));
			triangles[tc].NA[1] = (int) (255.0 * (n1->A - min_A_value) / (max_A_value - min_A_value));
			triangles[tc].NA[2] = (int) (255.0 * (n2->A - min_A_value) / (max_A_value - min_A_value));

/*			triangles[tc].X = ((n->X + n1->X + n2->X ) / 3 - min_X_value) /
						(max_X_value - min_X_value);
			triangles[tc].Y = ((n->Y + n1->Y + n2->Y) / 3 - min_Y_value) / 
			(max_Y_value - min_Y_value); */



			tc++;
    	              FI 
			nbl=nbl->next;
		OD
				/* IF (!n->fixed) THEN
		 	node *n2 = nbl->entry;
			IF (n->id < n1->id || n->id < n2->id) THEN
the_area += area_of_triangle(n,n1,n2);
		        triangles[tc].v0[0] = n->x;triangles[tc].v0[1] = n->y;triangles[tc].v0[2]  = n->z;
			triangles[tc].v1[0] = n1->x;triangles[tc].v1[1] = n1->y;triangles[tc].v1[2]  = n1->z;

			triangles[tc].v2[0] = n2->x;triangles[tc].v2[1] = n2->y;triangles[tc].v2[2]  =n2->z;

			triangles[tc].NX[0] = (int) (255.0 * (n->X - min_X_value) / (max_X_value - min_X_value));
			triangles[tc].NX[1] = (int) (255.0 * (n1->X - min_X_value) / (max_X_value - min_X_value));
			triangles[tc].NX[2] = (int) (255.0 * (n2->X - min_X_value) / (max_X_value - min_X_value));

			triangles[tc].NY[0] = (int) (255.0 * (n->Y - min_Y_value) / (max_Y_value - min_Y_value));
			triangles[tc].NY[1] = (int) (255.0 * (n1->Y - min_Y_value) / (max_Y_value - min_Y_value));
			triangles[tc].NY[2] = (int) (255.0 * (n2->Y - min_Y_value) / (max_Y_value - min_Y_value));

                        triangles[tc].NA[0] = (int) (255.0 * (n->A - min_A_value) / (max_A_value - min_A_value));
			triangles[tc].NA[1] = (int) (255.0 * (n1->A - min_A_value) / (max_A_value - min_A_value));
			triangles[tc].NA[2] = (int) (255.0 * (n2->A - min_A_value) / (max_A_value - min_A_value));


			tc++;
			FI
			FI */
		nl = nl->next;
	OD

	FOR (i=0; i<max_triangle; i++) DO
		int j;
		FOR (j=0; j<3; j++) DO
		IF (triangles[i].NX[j] < 0) THEN triangles[i].NX[j] = 0; FI
		IF (triangles[i].NY[j] < 0) THEN triangles[i].NY[j] = 0; FI
		IF (triangles[i].NA[j] < 0) THEN triangles[i].NA[j] = 0; FI
		IF (triangles[i].NX[j] > 255) THEN triangles[i].NX[j] = 255; FI
		IF (triangles[i].NY[j] > 255) THEN triangles[i].NY[j] = 255; FI
		IF (triangles[i].NA[j] > 255) THEN triangles[i].NA[j] = 255; FI
		OD
	OD

    fprintf(stderr, "Total area is at %g.\n", the_area);

END

void display_color_mapped_triangles(float rgbmap[256][3])
BEGIN
	int i;
	float c[4];
        float ci[4]; 
        unsigned short localX;
        FILE *g;
	node_list *nl;
        

  g = fopen("./rgbmap.dat","w");

  for(i=0; i<256; i++)
  {
    fprintf(g, "%d, %f, %f, %f\n",i,common_rgbmap[i][0],common_rgbmap[i][1],common_rgbmap[i][2]);
  }

  fclose(g);

	FOR (i=0; i<max_triangle; i++) DO
	/*	        IF (fill_triangles) c[0] = c[1] = c[2] = 0.0; */

    	                glPointSize(4.0);

                        glPolygonMode(GL_FRONT, GL_POINT);
                        glPolygonMode(GL_BACK, GL_POINT); 

		IF ( ! ((mode & 1) || (mode & 2) || (mode & 3)) ) THEN 


                        c[0] = 0.0; c[1] = 0.0; c[2] = 1.0; c[3]=1.0;
			glColor4fv(c); 
                        glBegin(GL_POLYGON);
			  glVertex3fv(triangles[i].v0); 
                          glVertex3fv(triangles[i].v1); 
                          glVertex3fv(triangles[i].v2); 
			glEnd();

		ELSE 
		/*		glLineWidth(1);*/
			IF (mode & 1) THEN  
/*************************************c[0]= triangles[i].X;*/
/*                          localX = (int) (255*triangles[i].X);   

                          ci[0] = (rgbmap[localX] [0]);
  			  ci[1] = (rgbmap[localX] [1]);
			  ci[2] = (rgbmap[localX] [2]);

     	                  glColor3fv(ci);  */
                         
		ci[3] = 1.0;

				glBegin(GL_POLYGON);
                                     ci[0] = rgbmap[triangles[i].NX[0]] [0];
				     ci[1] = rgbmap[triangles[i].NX[0]] [1];
				     ci[2] = rgbmap[triangles[i].NX[0]] [2];
                                     glColor3fv(ci);
                                  glVertex3fv(triangles[i].v0);
                          	     ci[0] = rgbmap[triangles[i].NX[1]] [0];
				     ci[1] = rgbmap[triangles[i].NX[1]] [1];
				     ci[2] = rgbmap[triangles[i].NX[1]] [2];
				     glColor3fv(ci); 
                                  glVertex3fv(triangles[i].v1);
				     ci[0] = rgbmap[triangles[i].NX[2]] [0];
				     ci[1] = rgbmap[triangles[i].NX[2]] [1];
				     ci[2] = rgbmap[triangles[i].NX[2]] [2];
				     glColor3fv(ci); 
                                  glVertex3fv(triangles[i].v2);
      			        glEnd(); 
                        FI

			IF (mode & 2) THEN
		ci[3] = 1.0;

				glBegin(GL_POLYGON);
                                     ci[0] = rgbmap[triangles[i].NY[0]] [0];
				     ci[1] = rgbmap[triangles[i].NY[0]] [1];
				     ci[2] = rgbmap[triangles[i].NY[0]] [2];
                                     glColor3fv(ci);
                                  glVertex3fv(triangles[i].v0);
                          	     ci[0] = rgbmap[triangles[i].NY[1]] [0];
				     ci[1] = rgbmap[triangles[i].NY[1]] [1];
				     ci[2] = rgbmap[triangles[i].NY[1]] [2];
				     glColor3fv(ci); 
                                  glVertex3fv(triangles[i].v1);
				     ci[0] = rgbmap[triangles[i].NY[2]] [0];
				     ci[1] = rgbmap[triangles[i].NY[2]] [1];
				     ci[2] = rgbmap[triangles[i].NY[2]] [2];
				     glColor3fv(ci); 
                                  glVertex3fv(triangles[i].v2);
      			        glEnd(); 
                        FI

			IF (mode & 3) THEN
		ci[3] = 1.0;

				glBegin(GL_POLYGON);
                                     ci[0] = rgbmap[triangles[i].NA[0]] [0];
				     ci[1] = rgbmap[triangles[i].NA[0]] [1];
				     ci[2] = rgbmap[triangles[i].NA[0]] [2];
                                     glColor3fv(ci);
                                  glVertex3fv(triangles[i].v0);
                          	     ci[0] = rgbmap[triangles[i].NA[1]] [0];
				     ci[1] = rgbmap[triangles[i].NA[1]] [1];
				     ci[2] = rgbmap[triangles[i].NA[1]] [2];
				     glColor3fv(ci); 
                                  glVertex3fv(triangles[i].v1);
				     ci[0] = rgbmap[triangles[i].NA[2]] [0];
				     ci[1] = rgbmap[triangles[i].NA[2]] [1];
				     ci[2] = rgbmap[triangles[i].NA[2]] [2];
				     glColor3fv(ci); 
                                  glVertex3fv(triangles[i].v2);
      			        glEnd(); 
                        FI
		 FI
           OD 

		/*#ifdef WHITEONBLACK */
		  /*		c[0] = c[1] = c[2] = 0.8;
#else
		c[0] = c[1] = c[2] = 0.2;
#endif

		IF ((mode & 4) || (mode & 8)) THEN
			long ci[3]; 

			ci[0] = ci[1] = ci[2] = 0;
			bgnclosedline();
			IF (mode & 4) THEN
#define value (triangles[i].NX[0])
				ci[0] +=rgbmap[value][0]; ci[1] += rgbmap[value][1]; ci[2]+= rgbmap[value][2];
			FI
			IF (mode & 8) THEN
#define value (triangles[i].NY[0])
				ci[0] +=rgbmap[value][0]; ci[1] += rgbmap[value][1]; ci[2]+= rgbmap[value][2];
			FI
			IF ( mode & 12) THEN
				ci[0] = ci[0]/2;ci[1] = ci[1]/2;ci[2] = ci[2]/2;
			FI
			c3i(ci); v3f(triangles[i].v0);

			IF (mode & 4) THEN
#define value (triangles[i].NX[1])
				ci[0] +=rgbmap[value][0]; ci[1] += rgbmap[value][1]; ci[2]+= rgbmap[value][2];
			FI
			IF (mode & 8) THEN
#define value (triangles[i].NY[1])
				ci[0] +=rgbmap[value][0]; ci[1] += rgbmap[value][1]; ci[2]+= rgbmap[value][2];
			FI
			IF ( mode & 12) THEN
				ci[0] = ci[0]/2;ci[1] = ci[1]/2;ci[2] = ci[2]/2;
			FI
			c3i(ci); v3f(triangles[i].v1);

			IF (mode & 4) THEN
#define value (triangles[i].NX[2])
				ci[0] +=rgbmap[value][0]; ci[1] += rgbmap[value][1]; ci[2]+= rgbmap[value][2];
			FI
			IF (mode & 8) THEN */
			/*#define value (triangles[i].NY[2])
			  ci[0] +=rgbmap[value][0]; ci[1] += rgbmap[value][1]; ci[2]+= rgbmap[value][2]; */
				/*			FI
			IF ( mode & 12) THEN
				ci[0] = ci[0]/2;ci[1] = ci[1]/2;ci[2] = ci[2]/2;
			FI
			c3i(ci); v3f(triangles[i].v2);
			endline();
		  
		FI



		OD */
       IF ( show_grid && ! ((mode & 4) || (mode & 8))) THEN
			glLineWidth(0.5);
                        c[0] = 1.0; c[1] = 1.0; c[2] = 1.0; c[3] = 1.0;
			glColor3fv(c); 

/*  original method, follows triangle (as interpreted in init_triangles) 
    edges		*/
                   FOR (i=0; i<max_triangle; i++) DO
		       glBegin(GL_LINE_LOOP);
			  glVertex3fv(triangles[i].v0); 
                          glVertex3fv(triangles[i].v1); 
                          glVertex3fv(triangles[i].v2); 
			glEnd();
		   OD 
/* 9/9/03 - try to draw only neighbour edges */

/*	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
			float v[3], u[3];
			v[0]=n->x; v[1]=n->y;v[2]=n->z; */
/* NEXT: use a loop to go through the neighbour list, and set the u components to each neighbour's x,y,z in turn. */
/*			glBegin(GL_LINES);
			  glVertex3fv(v);
                          glVertex3fv(u);
			  glEnd(); */
/* end the loop here. */

/*		nl = nl->next; 

			      OD*/
      	FI 
END

void display_triangles()
BEGIN
	display_color_mapped_triangles(common_rgbmap);
END

void display_fixed_nodes()
BEGIN
	node_list *nl;
	float c[3] = {0.0, 1.0, 0.0};
	float vt[3];

	nl = first_nl;
	WHILE(nl != NULL) DO
		node *n = nl->entry;
		IF (n->fixed) THEN
			float v[3];
			vt[0]=n->x; vt[1]=n->y; vt[2] = n->z-0.4;
			v[0]=n->x; v[1]=n->y;v[2]=n->z;
			glColor3fv(c);
                        glLineWidth(1);
			glBegin(GL_LINES);
			  glVertex3fv(v);
                          glVertex3fv(vt);
			glEnd();
		FI
		nl = nl->next;
	OD
END
			
void give_color_bar(float rgbmap[256][3])
BEGIN
       int run_color = 200;
       int i;
       float startx = -2.3;
       float starty = 3.7;
/*       float startz = 0.0; */
        

       float vll[2] = {startx, starty};
       float vul[2] = {startx, starty+0.3};
       float vur[2], vlr[2];
       float c[3]; 
       

	FOR(run_color= 0; run_color<256; run_color++) DO 
		c[0] = rgbmap[run_color][0]; 
		c[1] = rgbmap[run_color][1]; 
		c[2] = rgbmap[run_color][2];
        	glColor3fv(c);   

                vur[0] = startx+(float)(run_color)/64;
                vur[1] = vul[1];
/*                vur[2] =  startz; */
                vlr[0] = vur[0];
                vlr[1] = starty;
/*                vlr[2] = startz; */

                glBegin(GL_QUADS);
                  glVertex2fv(vll);
                  glVertex2fv(vul);
                  glVertex2fv(vur);
                  glVertex2fv(vlr);
               glEnd();

              for(i=0;i<2;i++)
              {
		vll[i]=vlr[i];
                vul[i]=vur[i];
	      }  

	      OD 
END

void draw_axes()
BEGIN
    glBegin(GL_LINES);
     glColor3f(0.0, 0.0, 1.0); /* z blue */
       glVertex3d(0.0, 0.0, 0.0);
       glVertex3d(0.0, 0.0, 3.0);
     glColor3f(1.0, 0.0, 0.0); /* x red */
       glVertex3d(0.0, 0.0, 0.0);
       glVertex3d(3.0, 0.0, 0.0);
     glColor3f(0.0, 1.0, 0.0);  /* y green */
       glVertex3d(0.0, 0.0, 0.0);
       glVertex3d(0.0, 3.0, 0.0);
     glEnd();   

END
		
// float comp_time;
void give_text_and_bar()
BEGIN
//	char msg[100];
/*	float colors[3] = {0,0,0}; */
/*        cmov2(-1.0,1.05); */
#ifdef WHITEONBLACK
/*        RGBcolor(230,230,230); */
#else
/*	RGBcolor(0,0,0); */
#endif

	IF (mode == 1) THEN
		sprintf(msg,"[%d-dump%d] Max z: %g", pid, picturecounter, max_z);
	ELIF (mode == 2) THEN
		sprintf(msg,"[%d-dump%d] Max z: %g", pid, picturecounter,max_z);
	ELSE
		sprintf(msg, "[%d-dump%d] Max z:%3.3f",pid,picturecounter,max_z);
		FI
	/*	cmov2(-0.9353, 1.05); */
	/*       	charstr(msg); */

	comp_time = delta_t*iterations;

	sprintf(msg, "Time: %f",  delta_t*iterations);
/*	cmov2(0.93, 1.05); */
/* 	charstr(msg); */


	give_color_bar(common_rgbmap);
        draw_axes();
END
			

void save_display_buffer()
BEGIN
	long x1, y1, x, y;
	long xsize, ysize;
	long x2, y2;
/*	winset(gid); getorigin(&x1,&y1);getsize(&xsize,&ysize);
	x2 = x1 + xsize; y2 = y1+ysize;
	IF (window_pixels_buffer == NULL) THEN
		window_pixels_buffer = (unsigned long *)
				malloc((x2-x1+1) * (y2-y1+1) * 
					sizeof(unsigned long));
	FI
	lrectread(0,0,xsize,ysize, window_pixels_buffer); */
END

void redraw_window()
BEGIN
	long x1, y1, x, y;
	long xsize, ysize;
	long x2, y2;
	IF (window_pixels_buffer == NULL) THEN
		return;
	FI
	/*	winset(gid); getorigin(&x1,&y1);getsize(&xsize,&ysize);
	x2 = x1 + xsize; y2 = y1+ysize;
	lrectwrite(0, 0, xsize, ysize, window_pixels_buffer); */
END

void display_grid()
BEGIN
	node_list	*nl;
	node *n;
	char msg[100];
	float colors[3] = {0,0,0};

     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/*	winset(gid); */
#ifdef WHITEONBLACK
	colors[0] = colors[1] = colors[2] = 0;
#else
	colors[0] = colors[1] = colors[2] = 0.9;
#endif
/*	reshapeviewport(); leave this out for starters, may have to 
	glViewport, glutReshapeFunc */

/*	glColor3fv(colors); */
/*	clear(); zclear();  */
//        give_text_and_bar();  
        draw_axes();
	glPushMatrix();	
        	glRotatef(rotate_x, 1.0, 0.0, 0.0);
        	glRotatef(rotate_y, 0.0, 1.0, 0.0);
        	glRotatef(rotate_z, 0.0, 0.0, 1.0);
	
		glScalef(scale_value, scale_value, scale_value);
		display_triangles();
		display_fixed_nodes();
	glPopMatrix();
    
//        glFlush();

/*	give_text_and_bar(); */

/*	save_display_buffer(); */
END		 


void jump_to_numbered_picture(int jumpnum)
BEGIN
	char filename[80];
	
	sprintf(filename, "%s/dump%d", dumpdir, jumpnum);
	IF ( undump(filename) ) THEN
		picturecounter = jumpnum;
	init_triangles();
//  Dall not needed, if init_triangles updated          display_grid();
		printcounter = 0;
	FI
END

void display_previous_picture()
BEGIN
	char filename[80];
	IF (picturecounter == 0) THEN
		return;
	FI
	
	picturecounter --;
	sprintf(filename, "%s/dump%d", dumpdir, picturecounter);
	IF ( undump(filename) ) THEN
		init_triangles();
// Call not needed, if init_trianges updated		display_grid();
		printcounter = 0;
	ELSE
		picturecounter--;
		FI
END
void display_next_picture()
BEGIN
	char filename[80];
	
	picturecounter ++;
	sprintf(filename, "%s/dump%d", dumpdir, picturecounter);
	IF ( undump(filename) ) THEN
		init_triangles();
// Call not needed, if init_triangles updated		display_grid();
		printcounter = 0;
	ELSE
		picturecounter--;
		FI 
END

static void Key( unsigned char key, int x, int y )
{
  (void) x;
  (void) y;

  switch (key) {
        case 27:	
	  /*       glutDestroyWindow("Growth Simulation");  dumps core  */
           exit(0);
	   break; 
        case 'P': case 'p':
	  print_picture();
          break;
        case 'c': case 'C':
          printf("check\n");
          break;
        case '+':
          scale_value+=0.2;
          display_grid();
	  //          printf("scale_value is %f\n",scale_value);
	break;
        case '-':
          if(scale_value-0.2>0.0)
	  {
            scale_value-=0.2;
            display_grid();
	    //   printf("scale_value is %f\n",scale_value);
	  }
        break;
  }
}

void init_display(int argc, char** argv)
BEGIN
	char windowname[80];
	char filename[80];

//        GlowViewManipulator* _manip; 

	sprintf(filename, "%s/dump%d", dumpdir, picturecounter); 
        printf("undumping from '%s'\n", filename);
	sprintf(windowname, "Growth: Glut Window");

	fill_triangles = TRUE;
	undump(filename);
	count_node_degrees();
	init_rgb_maps();
	init_triangles();

        Glow::Init(argc, argv);
        glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
        glutInitWindowSize(400, 400);
        glutInitWindowPosition(10, 10);
        windowID = glutCreateWindow(windowname);
/* 7/7/03 -  using glut method for window creation - how does this square with mandelgizmo, C++ way of window creation, in which a Manipulator can be tagged to the window? - Better: pop up a Gizmo window by default, plus allowing button option, which will take place of main (glut) Window. */
       glutKeyboardFunc( Key );

//     _manip = new GlowViewManipulator(windowID,GlowViewManipulatorParams::defaults);


        glEnable(GL_DEPTH_TEST); 
        glEnable(GL_LINE_SMOOTH);
        glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); /*doesn't seem to matter */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDisable(GL_BLEND); 
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glShadeModel(GL_FLAT);
        glClearDepth(1.0);    
/*        glDepthFunc(GL_LEQUAL); */
/*        glDisable(GL_DEPTH_TEST);  */
        
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
    /*    gluPerspective(90.0, 1.0, 0.5, 5.0); */
        glOrtho(-4.0, 4.0, -4.0, 4.0, -100.0, 100.0);  
    /*    glFrustum(-2.0, 2.0, -2.0, 2.0, 1.0, 2.0); */
        glMatrixMode(GL_MODELVIEW);
        gluLookAt(0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0); 
    

/*	keepaspect(1,1);
        minsize(650,650); */


/*        gid = swinopen(dform->window); 2/8/02 */
/*	winposition(picture_box->x+4, picture_box->x+picture_box->w-4,
	picture_box->y+4, picture_box->y+picture_box->h-4); 2/8/02 */

/*	RGBmode();
	zbuffer(TRUE); */
/*
	doublebuffer();
*/
/*	gconfig(); 2/8/02*/
/*
	qdevice(WINSHUT); qdevice(WINQUIT); qdevice(MOUSEX); qdevice(MOUSEY);
        qdevice(LEFTMOUSE); qdevice(RIGHTMOUSE); qdevice(MIDDLEMOUSE);
        qdevice(ESCKEY); qdevice(KEYBD); qdevice(WINFREEZE);
	qdevice(WINTHAW);qdevice(REDRAW);
*/
/*        perspective(400, 1.0, 0.001, 100000.0); 
	  translate(0.0,0.0,-3.0); rotate(900,'z'); this line may need 
                    translation to Opengl if things don't display right */


 	rotate_x = 0; rotate_y = 0; rotate_z = 0;

 	xcoor = 0; ycoor = 0;
//       scale_value = 0.93;
/*#ifdef WHITEONBLACK
	RGBcolor(0,0,0);
#else
	RGBcolor(255,255,255);
#endif
clear();zclear(); 2/8/02 */
   glutDisplayFunc(display_grid);

   new DispButtons(windowID);

// New line to run mandelbrot demo from display, 4/16/03

//   new MandelWind (new MandelData);


   Glow::MainLoop();
END

/*  not used ...
 void print_postscript_picture()
BEGIN
	long xorigin, yorigin;
	long xsize, ysize;
	long xup, yup;
	char command[100];
	getorigin(&xorigin,&yorigin);	
	getsize(&xsize,&ysize); 
 	xup = xorigin + xsize; yup = yorigin+ysize;

 	sprintf(command, "scrsave o.rgb %d %d %d %d ", xorigin, xup,
				yorigin + 50 , yup);
	system(command);

	sprintf(command, "/bin/rm -f o.ps ; tops -rgb o.rgb  > o.ps");
	
	system(command);
END */

/*void display_grid_for_printing()
BEGIN
	node_list	*nl;
	node *n;
	char msg[100];
	float colors[3] = {0,0,0};
	int i,j;
	int tempprint_rgbmap[256][3];

	FOR (i=0; i<256; i++) DO
		FOR (j=0; j<3; j++) DO
			tempprint_rgbmap[i][j] = print_rgbmap[i][j];
		OD
	OD
 
	IF (((mode & 1) && (min_X_value<0.0)) ||
	    ((mode & 2) && (min_Y_value<0.0))) THEN
		REAL a,b;
		int zeroval;

		IF (mode & 1) THEN
			a = (max_X_value - min_X_value) / 255;
			b = min_X_value;
		FI
		IF (mode & 2) THEN
			a = (max_Y_value - min_Y_value) / 255;
			b = min_Y_value;
		FI
		
		zeroval = (int) ( (double) -b/a);
#define CHANGEM1 1
#define CHANGEP1 1
#ifdef CHANGEM1
		tempprint_rgbmap[zeroval-1][0] = 0;
		tempprint_rgbmap[zeroval-1][1] = 0;
		tempprint_rgbmap[zeroval-1][2] = 0;
#endif
		tempprint_rgbmap[zeroval][0] = 0;
		tempprint_rgbmap[zeroval][1] = 0;
		tempprint_rgbmap[zeroval][2] = 0;
#ifdef CHANGEP1
		tempprint_rgbmap[zeroval+1][0] = 0;
		tempprint_rgbmap[zeroval+1][1] = 0;
		tempprint_rgbmap[zeroval+1][2] = 0;
		#endif */
/*
#define limit_255(a) if ( (a)>255) a = 255;
		limit_255(tempprint_rgbmap[zeroval-1][0]);
		limit_255(tempprint_rgbmap[zeroval-1][1]);
		limit_255(tempprint_rgbmap[zeroval-1][2]);
		limit_255(tempprint_rgbmap[zeroval+1][0]);
		limit_255(tempprint_rgbmap[zeroval+1][1]);
		limit_255(tempprint_rgbmap[zeroval+1][2]);
*/
/*	FI */

	/* is the following necessary? 10/28/02 */
/*       	winset(gid);
	colors[0] = colors[1] = colors[2] = 0.0;
	reshapeviewport();
	c3f(colors);
	clear(); zclear(); 
	pushmatrix();	
        	rotate(rotate_x,'x');
        	rotate(rotate_y,'y');
        	rotate(rotate_z,'z');
	
		scale(scale_value, scale_value, scale_value);
		display_color_mapped_triangles(common_rgbmap);
	popmatrix();
 	give_color_bar(common_rgbmap); 
	END	*/	  

void print_picture()
BEGIN
	long x1, y1, x, y;
	long xsize, ysize;
	long x2, y2;
	char command[100];
	char filename[100];
	unsigned long *parray;
	FILE *f;

/* 12/30/02 - changing this function to be like  the IAM system, display1002.c */

/*	display_grid_for_printing(); called in IAM version */
        gid = glutGetWindow();
 	glutSetWindow(gizmowindname); 
        x1 = glutGet(GLUT_WINDOW_X);
        y1 = glutGet(GLUT_WINDOW_Y);
        xsize = glutGet(GLUT_WINDOW_WIDTH);
        ysize = glutGet(GLUT_WINDOW_HEIGHT);
/*	x2 = x1 + xsize; 
        y2 = y1+ysize; */

     parray = (unsigned long *) malloc( (xsize+1) * (ysize+1) * sizeof(unsigned long));

         printf("Printing dimensions are %d X %d\n",xsize, ysize);
/* Note: glReadPixels' first 2 arguments are in *window* coords */
	glReadPixels(0,0,xsize,ysize, GL_RGBA, GL_UNSIGNED_BYTE, parray);
	sprintf(filename, "/tmp/p.ps");
	f = fopen(filename, "w");
	IF (f == NULL) THEN
		fprintf(stderr, "PRINTERROR: could not open %s for write.\n",
                filename);
		free(parray);
		return;
	FI
        int ytrans=792-ysize;
	fprintf(f, "%%!PS-Adobe-3.0\n");
        fprintf(f, "/picstr %d string def\n", (xsize+1)*4); 
	fprintf(f, "0 %d translate\n%d %d scale\n", ytrans, xsize, ysize);
	fprintf(f, "%d %d 8 [%d 0 0 %d 0 0 ]\n",xsize, ysize+1,
		xsize+1, ysize+1); 
	fprintf(f, "{ currentfile picstr readhexstring pop } false 3\ncolorimage\n\n");
	fprintf(f, "\n");
	
 	FOR (y=0; y<=ysize; y++) DO
	 	FOR (x=0; x<=xsize;x++) DO
 			int red, green, blue;
			unsigned long longrgb = parray[x + y*(xsize+1)];
	 		red = longrgb & 0xff ;
		 	green = ((longrgb >> 8 ) & 0xff);
			blue = ((longrgb >> 16 ) & 0xff);
		 	fprintf(f, "%x%x", (red&0xf0)>>4, red&0x0f);
		 	fprintf(f, "%x%x", (green&0xf0)>>4, green&0x0f);
		 	fprintf(f, "%x%x", (blue&0xf0)>>4, blue&0x0f);
 		OD
 	OD
 	fprintf(f, "\n\nshowpage\n");
 	fclose(f); free(parray);
 	fprintf(stderr, "PRINT: wrote image to %s\n", filename);




	END 

/*void print_picture_gif()
BEGIN
	long x1, y1, x, y;
	long xsize, ysize;
	long x2, y2;
	char command[100];
	char filename[100];
	unsigned long *parray;
	FILE *f;

	display_grid_for_printing();
	winset(gid);
	getorigin(&x1,&y1);	
	getsize(&xsize,&ysize);
	x2 = x1 + xsize; y2 = y1+ysize;

	parray = (unsigned long *) malloc( (x2-x1+1) * (y2-y1+1) * sizeof(unsigned long));

printf("Printing dimensions are %d X %d\n",x2-x1+1, y2-y1+1);
	readdisplay(x1,y1,x2,y2, parray, RD_FREEZE);
	sprintf(filename, "/tmp/p.ppm");
	f = fopen(filename, "w");
	IF (f == NULL) THEN
		fprintf(stderr, "PRINTERROR: could not open %s for write.\n",
                filename);
		free(parray);
		return;
	FI
	fprintf(f, "P6\n");
	fprintf(f, "%d %d\n", xsize+1, ysize+1);
	fprintf(f, "255\n");
	FOR (y=ysize; y>=0; y--) DO
		FOR (x=0; x<=xsize;x++) DO
			int red, green, blue;
			unsigned long longrgb = parray[x + y*(xsize+1)];
			red = longrgb % 256; 
			green = ((longrgb / 256 ) % 256); 
			blue = ((longrgb / 65536) % 256);
			fprintf(f, "%c%c%c", red, green, blue);
		OD
	OD
	fclose(f); free(parray);
	fprintf(stderr, "PRINT: wrote image to %s\n", filename);

	sprintf(command, "/home/swehner/bin/ppmquant 25 /tmp/p.ppm > /tmp/p1.ppm");
	system(command);
	fprintf(stderr, "PRINT: produced reduced ppm-file /tmp/p1.ppm\n");
	sprintf(command, "/home/swehner/bin/ppmtogif /tmp/p1.ppm > /tmp/p_%d_%d.gif", picturecounter, printcounter);
	system(command);
	fprintf(stderr, "PRINT: produced gif file /tmp/p_%d_%d.gif\n", 
	picturecounter, printcounter);
	printcounter ++;
	END*/

/*void print_actual_picture()
BEGIN
	long x1, y1, x, y;
	long xsize, ysize;
	long x2, y2;
	char filename[100];
	unsigned long *parray;
	FILE *f;

	winset(gid);
	getorigin(&x1,&y1);	
	getsize(&xsize,&ysize);
	x2 = x1 + xsize; y2 = y1+ysize;

	parray = (unsigned long *) malloc( (x2-x1+1) * (y2-y1+1) * sizeof(unsigned long));

	readdisplay(x1,y1,x2,y2, parray, RD_FREEZE);
	sprintf(filename, "/tmp/p%d.ppm", picturecounter);
	f = fopen(filename, "w");
	IF (f == NULL) THEN
		fprintf(stderr, "PRINTERROR: could not open %s for write.\n",
                filename);
		free(parray);
		return;
	FI
	fprintf(f, "P6\n");
	fprintf(f, "%d %d\n", xsize+1, ysize+1);
	fprintf(f, "255\n");
	FOR (y=ysize; y>=0; y--) DO
		FOR (x=0; x<=xsize;x++) DO
			int red, green, blue;
			unsigned long longrgb = parray[x + y*(xsize+1)];
			red = longrgb % 256; 
			green = ((longrgb / 256 ) % 256); 
			blue = ((longrgb / 65536) % 256);
			fprintf(f, "%c%c%c", red, green, blue);
		OD
	OD
	fclose(f); free(parray);
	fprintf(stderr, "PRINT: wrote image to %s\n", filename);
	END*/
void redraw_event_action()
BEGIN
#ifdef WHITEONBLACK
	/*	RGBcolor(255,255,255); */
#else
	/*	RGBcolor(255,255,255); */
#endif

	display_grid();
END



/*void interpret_users_inputs(void)
BEGIN
	Device dev;
	short val;
	short new_mode;
	
	dev = qread(&val);
	WHILE (TRUE)
	DO
		switch(dev)
		BEGIN
		   case WINFREEZE: 
			window_is_visible = FALSE;
			return; */
/********************* no escaping!
                case ESCKEY:            
			active=FALSE; display_grid();
                        WHILE(qtest()) DO 
				dev=qread(&val); 
			OD
                        return; 
***************************************/
/*		case KEYBD: 
		
        		IF (val == 'd') THEN
				count_node_degrees();
				display_grid();
			ELIF (val == 'g') THEN 
				show_grid = ! show_grid;
				return;
			ELIF (val == 'Q') THEN
				user_stops_exe=TRUE;
				return; 
			ELIF (val == 'r') THEN
				display_grid();
			ELIF (val == 'p') THEN
				display_previous_picture();
			ELIF (val == 'n') THEN
				display_next_picture();
			ELIF (val == 'f') THEN
				fill_triangles = ! fill_triangles;
				display_grid();
			ELIF (val == 'P') THEN
				print_picture();

			ELIF (val == 'i') THEN
				printf("rotx=%d roty=%d rotz=%d scale=%f\n",
				rotate_x, rotate_y, rotate_z, scale_value);
			ELIF (val == 'z') THEN
				rotate_x = 0; rotate_y = 600; rotate_z = 0;
				scale_value = 0.93;
			ELIF (val == 'h' || val == '?') THEN
printf("\n================== HELP =======================\n");
printf("\tn   show next picture\n");
printf("\tp   show previous picture\n");
printf("\tr   refresh display\n");
printf("\td   give maximal node degree, refresh display\n");
printf("\tf   toggle fill triangles according to X/Y-value\n"); 
printf("\tg   toggle grid display\n");
printf("\tP   produce postscript file o.ps\n");
printf("\tz   reset rotation/scale values\n");
printf("\ti   give rotation/scale values\n");
printf("\tQ   quit\n");
printf("\th,? give this list\n");
ELSE
				new_mode = val - 48;
				IF (new_mode>=0 && new_mode<48)THEN
				   mode = new_mode;
				   IF (new_mode == 5) THEN
					mode = 8;
			  	   ELIF (new_mode == 6) THEN
					mode = 12;
				   FI
				FI
			FI
			break;   
		case WINQUIT:		
			user_stops_exe = TRUE; return;
		case WINSHUT: 		
			user_stops_exe = TRUE; return;
		case REDRAW:
		case WINTHAW: 		
			redraw_event_action();
			break;
		case LEFTMOUSE: BEGIN
			long xorigin, yorigin;
			long xsize, ysize;
			long xcoor, ycoor;
			getorigin(&xorigin,&yorigin);	
			getsize(&xsize,&ysize);
			xcoor = getvaluator(MOUSEX) - xorigin;
			ycoor = getvaluator(MOUSEY) - yorigin;
			IF (xcoor > ycoor) THEN
				rotate_x = 3600 * (float)(xcoor) / xsize;
			ELSE
				scale_value = 10 * (float)(ycoor) / ysize;
			FI
				END break;

		case MIDDLEMOUSE: BEGIN
			long xorigin, yorigin;
			long xsize, ysize;
			long xcoor, ycoor;
			getorigin(&xorigin,&yorigin);	
			getsize(&xsize,&ysize);
			xcoor = getvaluator(MOUSEX) - xorigin;
			ycoor = getvaluator(MOUSEY) - yorigin;
			IF (xcoor > ycoor) THEN
				rotate_y = 3600 * (float)(xcoor) / xsize;
			ELSE
				scale_value =  (float)(ycoor) / ysize;
			FI
				END break;
		case RIGHTMOUSE: BEGIN
			long xorigin, yorigin;
			long xsize, ysize;
			long xcoor, ycoor;
			getorigin(&xorigin,&yorigin);	
			getsize(&xsize,&ysize);
			xcoor = getvaluator(MOUSEX) - xorigin;
			ycoor = getvaluator(MOUSEY) - yorigin;
			IF (xcoor > ycoor) THEN
				rotate_z = 3600 * (float)(xcoor) / xsize;
			ELSE
				scale_value = 0.1 * (float)(ycoor) / ysize;
			FI
				END break;
		END
	        dev = qread(&val);
		OD 
END */

/*void check_d */
