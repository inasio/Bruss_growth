#include "BM.h"
#include <stdio.h>

#define GLOBAL extern
#include "globals.h"

#include <math.h>


/*
 *
Function add_point

Function add_point takes three integer arguments. The circle number,
the sequence number and the member number.


Coordinates of the new point are calculated, and set.

Arguments: three ints
Returns: void
 *
 */


void add_point(int circleno, int seqno, int memno)
BEGIN
	REAL radius = (REAL) (border_order - circleno - 1) / (border_order-1);

	REAL angleseq = seqno * 2 * M_PI / center_order;
	REAL anglemem = (REAL) (memno) / (border_order - circleno - 1)
				* 2 * M_PI / center_order;

	x[next_point] = radius * cos(angleseq + anglemem);
	y[next_point] = radius * sin(angleseq + anglemem);

	next_point++;
END

/*
 *
Function
	make_first_sequence

Function make_first_sequence defines coordinates of points along the
outer circle.
 *
 */

void make_first_sequence()
BEGIN

	int i,j;


	x[0] = 1.0;
	y[0] = 0.0;
	next_point = 1;

	FOR (i=0; i<center_order-1; i++) DO
		FOR (j = 1; j<border_order; j++) DO
			add_point(0, i, j);
		OD
	OD

	FOR (j = 1; j<border_order-1; j++) DO
		add_point(0, center_order-1, j);
	OD
	last_sequence_first = 0;
END


/*
 *
Function
	add_sequence

Places nodes of  the "s"th sequence
 *
 */
void add_sequence(int s)
BEGIN
	int i,j,below;
	int this_sequence_first;
	REAL r;

	below = last_sequence_first;
	this_sequence_first = next_point;

	r = 1.0 - (REAL) s / (border_order-1);

	add_point(s, 0,0);
	FOR (i=0; i<center_order-1; i++) DO
		FOR (j = 1; j<border_order-s; j++) DO
			add_point(s,i,j);
			below++;
		OD
		below++;
	OD

	FOR (j = 1; j<border_order-s-1; j++) DO
		add_point(s,i,j);
		below++;
	OD
	below++;

	last_sequence_first = this_sequence_first;
END

/*
 *
Function add_last_element

places the inner most node
 *
 */
void add_last_element(void)
BEGIN
	int i, below;

	x[next_point] = 0.0;
	y[next_point] = 0.0;
	next_point++;
END
