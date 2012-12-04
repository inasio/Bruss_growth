#include "BM.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>

#include "types.h"
#define GLOBAL extern
#include "globals.h"


#include "setup_matrix.h"

#include "read_parameters.h"
#include "init_surface.h"
#include "init_distribution.h"
#ifdef DISPLAYING
#include "display.h"
#include <gl/gl.h>
#endif

#include "memory.h"
#include "refine.h"
#include "reaction_diffusion.h"
#include "triangle.h"

/*
 *
Macros mark and changemark

	Will reuse the arrays previous_X and previous_Y for
	markers

	These arrays are only used in the reaction-diffusion step

	Only need shorts!
 *
 */


#define mark(NODE) (* ( (int *)(& (NODE)->previous_X)))
#define changemark(NODE) (* ( (int *)(& (NODE)->previous_Y)))

/*
 *
Macros  NOTMARKED, MARKED, NEW, NOTCHANGED, CHANGED

	Marker-values
 *
 */

#define NOTMARKED  0
#define MARKED     1
#define NEW	   2

#define NOTCHANGED 0
#define CHANGED    1
#define DEGCHANGE  2


/*
 *
Function
	find_maximal_initial_length

will look at all edges and find the longest length. Actually it
only determines the square of that number.

sets global variable
	max_initial_length_squared.

Arguments: none
Returns: void

 *
 */
void find_maximal_initial_length()
BEGIN
	node_list *nl;

	max_initial_length_squared = 0.0;

	nl = first_nl;

	WHILE (nl != NULL) DO
		node *n = nl->entry;
		double x,y,z;
		node_list *nbl = n->nl;
		x = n->x; y = n->y; z = n->z;

		WHILE (nbl != NULL) DO
			node *nb = nbl->entry;
			double dx,dy,dz;
			double distance;
			dx = x-nb->x; dy = y-nb->y; dz = z-nb->z;

			distance = dx*dx+dy*dy+dz*dz;
			IF (distance > max_initial_length_squared) THEN
				max_initial_length_squared = distance;
			FI

			nbl = nbl->next;
		OD

		nl = nl->next;
	OD
      return;

END

/*
 *
Function
	extend_marking_for

Function  extend_marking_for looks at one node and checks whether
in its neighbourhood there are four nodes in sequence so that
two nodes are marked and two are not:

	*   .  .    *

	 \  |  |   /
          \ |  |  /
           \|  | /
	      N

* denotes a marked neighbour, . an unmarked neighbour.

If there are such neighbours the node is marked ( mark(n) := TRUE )

See documentation for function extend_markings
for the purpose.

Argument: node pointer
Returns: TRUE if node was marked
	 FALSE if node was not marked.
 *
 */
short extend_marking_for(node *n)
BEGIN
	node_list *nl;

	short neighbours_marked[50];
	int n_counter;

	IF (mark(n) == MARKED) THEN
		return FALSE;
	FI

	nl = n -> nl;
	n_counter = 0;
	WHILE (nl != NULL) DO
		neighbours_marked[n_counter++] = (mark(nl->entry) == MARKED);
		nl = nl -> next;
	OD
	FOR (n_counter = 0; n_counter < n->degree; n_counter++) DO
		short d = n->degree;

		IF ( neighbours_marked[n_counter      ] &&
		     !neighbours_marked[(n_counter+1)%d] &&
		     !neighbours_marked[(n_counter+2)%d] &&
		     neighbours_marked[(n_counter+3)%d] &&
                     n->degree == 6) THEN
			mark(n)= MARKED;
			return TRUE;
		FI
	OD

	return FALSE;
END


/*
 *
Function
	extend_markings()

Function extend_markings marks nodes until a region of nodes is marked,
so that the grid may be extended according to the following method:

If a triangle has a marked node, then it will be "cut" into four
triangles.

A triangle is cut by placing new nodes at the middle of each side and
connecting these new nodes.

Each side of a triangle of the original mesh is either on the boundary,
or part of two triangles.  There are three cases for each new node.

	1. it lies on the boundary
	2. it lies on a side both of whose triangles will be cut
	3. it lies only on one triangle to be cut (the other triangle will
	   not be cut)

The first and second case are not interesting here: the new triangles
on which the new nodes lie are easy to identify.

In the third case the grid will be refined as follows:  A new node lies
on a side of two triangles: One of these triangles has a marked node,
and the other triangle has an unmarked node (call it UN) which is not
endpoint of the side which the new node lies on.  An additional edge
from the new node to UN will be defined, so that the new node lies on 5
triangles.

Case 1:
Before refinement:
   M-------------B
    \           /
     \         /
      \       /
       \     n
        \   /
         \ /
          B
A marked node with label M which lies close to boundary nodes, labeled B.
A new node labeled n is on the boundary.
After refinement
   M------N------B
    \    /\     /
     \  /  \   /
      \/    \ /
       N-----n
        \   /
         \ /
          B
New nodes N and n are connected to form triangles. (Case 1 is
interested only in the node that lies on the boundary side of the original
triangle)

Case 2:
Before refinement
   U-------------M
    \           / \
     \         /   \
      \       /     \
       \     n       \
        \   /         \
         \ /           \
          U-------------U
A new node n lies on two triangles to be cut.
After refinement:
   U------N-----M
    \     /\    /\
     \   /  \  /  \
      \ /    \/    \
       N-----n------N
        \   / \   /  \
         \ /   \ /    \
          U-----N------U
The new node n is connected with other new nodes labeled N to lie on
six new triangles.

Case 3:
Before refinement:
   U-------------U
    \           / \
     \         /   \
      \       n     \
       \     /       \
        \   /         \
         \ /           \
          U-------------M
A new node labeled n lies on one triangle with just one marked corner,
and another triangle with no marked corner (label U)
   C------------U
    \ \         /\
     \  \      /  \
      \   \   /    \
       \    \n------N
        \   / \   /  \
         \ /   \ /    \
          U-----N------M

The new node n is connected to other new nodes N in the right handside
triangle and to the unmarked corner node labeled C in the left handside
triangle. It lies on five new triangles.

--------------------------------------------------------
The condition in extend_markings_for_node ensures that this method can
be carried out throughout the mesh.

Arguments: none
Returns: void
 *
 */

void extend_markings(void)
BEGIN
	short markings_changed;
	do {
		node_list *nl;
		markings_changed = FALSE;

		nl = first_nl;
		WHILE(nl != NULL) DO
			node *n = nl->entry;

			IF ( extend_marking_for(n) ) THEN
				markings_changed = TRUE;
			FI
			nl = nl -> next;
		OD
	} while (markings_changed);
      return;
END

/*
 *
Function
	needs_to_be_marked

Function needs_to_be_marked returns true if a node lies on a side that
is deemed to be too long, false otherwise. Too long refers to the
length_tolerance parameter, and the maximal initial length of sides in
the initial triangulation.

Arguments: pointer to node
Returns:
	TRUE if node should be marked
	FALSE otherwise
 *
 */

short needs_to_be_marked(node *n)
BEGIN
	node_list *nbl;
	double x,y,z;
	x = n->x; y = n->y; z = n->z;
	nbl = n->nl;
	WHILE (nbl != NULL) DO
		node *nb = nbl->entry;
		double dx, dy, dz;
		dx = x-nb->x; dy = y-nb->y; dz = z-nb->z;
		IF (dx*dx+dy*dy+dz*dz > length_tolerance*max_initial_length_squared) THEN
			return TRUE;
		FI
		nbl = nbl->next;
	OD
	return FALSE;
END
/*
 *
Function
	mark_nodes

Function mark_nodes marks those nodes that "need_to_be_marked"
Also initializes the mark,changemark to NOTMARKED/NOTCHANGED

Arguments: none
Returns:
	TRUE if some node was marked
	FALSE if no nodes were marked
 *
 */

short mark_nodes()
BEGIN
	short marked;
	node_list *nl;

	nl = first_nl;
	marked = FALSE;
	WHILE(nl != NULL) DO
		node *n = nl->entry;
		IF (needs_to_be_marked(n) && n->degree == 6) THEN
			mark(n) = MARKED;
			marked = TRUE;
			changemark(n) = CHANGED;
                      fprintf(stderr,"%d marked\n",n->id);
		ELSE
			mark(n) = NOTMARKED;
			changemark(n) = NOTCHANGED;
		FI


		nl = nl -> next;
	OD
	IF (!marked) THEN
		return (FALSE);
	FI

       	extend_markings();

	return(TRUE);
END

/*
 *
Function
	node_to_the_left

Function node_to_the_left takes two pointers to nodes as arguments: one
pointer to a node "n" and one pointer "neighbour" to one of "n"'s
neighbours.  It determines the pointer to that node that occurs just
before the "neighbour" in the neighbourhood list of node "n".
According to the usual convention of this package, this node lies to
the left of "neighbour" in the neighbourhood of "n"

The function exits with an error message if no such neighbour could be
found.

Arguments: pointer to two nodes.
Returns: pointer to node which lies to the left
 *
 */

node *node_to_the_left(node *n, node *neighbour)
BEGIN
	node_list *nl;
	node *left_neighbour;


	nl = n->nl;
	left_neighbour = nl->entry;
	IF (left_neighbour == neighbour) THEN
		WHILE (nl != NULL) DO
			left_neighbour = nl->entry;
			nl = nl -> next;
		OD
		return left_neighbour; /* which is the last neighbour in
					the list. */
	FI

	nl = nl -> next;
	WHILE (nl != NULL) DO
		IF (nl -> entry == neighbour) THEN

			return left_neighbour;
		ELSE
			left_neighbour = nl->entry;
		FI
		nl = nl -> next;
	OD


	printf("SRD_Growth: Could not find left neighbour of node %d from node %d\n",n->id, neighbour->id);
	EXIT;
	return left_neighbour; /* to preempt error message by compiler
					need to return a pointer.
				*/
END

/*
 *
Function
	replace_neighbour

Function replace_neighbour changes the neighbourhood list of the first
arguments node, so that instead of the second arguments node it has
a neighbour given by the third argument.

Sets the changemark of the first argument node to CHANGED.

Exits with an error message if the second node given is not in the
neighbour list of the first node given.

Arguments: three pointers to nodes.
Returns: void
 *
 */
void replace_neighbour(node *n, node *old_neighbour, node *new_neighbour)
BEGIN
	node_list *nl;
        int i,k;

	if(changemark(n) != DEGCHANGE) changemark(n) = CHANGED;


       nl = n->nl;
	WHILE (nl->entry != old_neighbour) DO
		nl = nl -> next;
		IF (nl == NULL) THEN
			fprintf(stderr, "SRD_Growth[%d]: Could not find neighbour %d "
					"to replace with %d "
				    "for node %d!\n",pid, old_neighbour->id, new_neighbour->id, n->id);
			EXIT;
		FI
	OD

	nl->entry = new_neighbour;
      return;

END

/*
 *
Function
	give_four_neighbours

Function give_four_neighbours takes five node pointers and sets up
the neighbour list of the first argument to be the four other nodes
(in the order given)

Sets the changemark of the first argument node to CHANGED.

Arguments: pointers to five nodes
Returns: void
 *
 */
void give_four_neighbours(node *n, node *n1, node *n2, node *n3, node *n4)
BEGIN
	node_list *newnl, *nl;
        int i;


	IF (n1->fixed && n4->fixed) THEN
		n->fixed = TRUE;
	FI

	if(changemark(n) != DEGCHANGE) changemark(n) = CHANGED;

	n->nl = new_node_list(); n->nl->entry = n1;
	n->nl->next = new_node_list(); n->nl->next->entry = n2;
	n->nl->next->next = new_node_list(); n->nl->next->next->entry = n3;
	n->nl->next->next->next = new_node_list(); n->nl->next->next->next->entry = n4;



      set_degree_for_node(n);

      return;

END

/*
 *
Function
	add_neighbours

Function add_neighbours (should be called add_two_neighbours) takes four
pointers to node, and inserts two
nodes in the neigbourhood list of the first node to the "right" of the
second node given, in the order specified.

Sets the changemark of the first argument node to CHANGED.

Exits if not possible.

Arguments: pointers to four nodes.
Returns: void
 *
 */
void add_neighbours(node *n, node *old_neighbour, node *new_n1, node *new_n2)
BEGIN
	node_list *nl, *new_nl1, *new_nl2;
        int i;


	if(changemark(n) != DEGCHANGE) changemark(n) = CHANGED;


	nl = n->nl;
	WHILE (nl->entry != old_neighbour) DO
		nl = nl->next;
		IF (nl == NULL) THEN
			fprintf(stderr, "SRD_Growth[%d]: Could not find neighbour %d "
					"to add neighbours %d and %d "
					"for node %d!\n",pid, old_neighbour->id, new_n1->id, new_n2->id,
							 n->id);
			EXIT;
		FI
	OD

	new_nl1 = new_node_list();
	new_nl2 = new_node_list();
	new_nl1->entry = new_n1;
	new_nl2->entry = new_n2;

	new_nl2->next = nl->next;
	new_nl1->next = new_nl2;
	nl->next = new_nl1;


      set_degree_for_node(n);

      return;

END

/*
 *
Function
	add_three_nodes

Function add_three_nodes takes three pointers to nodes and, assuming
these form a triangle, creates three new nodes placed at the center of
the sides formed by the three given nodes. The function sets up the
neighbour lists of all six nodes accordingly.

The new nodes created are marked as NEW.

It calls the functions replace_neighbour and give_four_neighbours and
so the function might cause the program to exit, if the given nodes do
not form a triangle.

Arguments: pointers to three nodes
Returns: void
 *
 */



void add_three_nodes(node *n1, node *n2, node *n3)
BEGIN
	node *new_n1, *new_n2, *new_n3;

	new_n1 = new_node();
	new_n2 = new_node();
	new_n3 = new_node();

	mark(new_n1) = NEW;
	mark(new_n2) = NEW;
	mark(new_n3) = NEW;

	replace_neighbour(n1, n2, new_n1);
	replace_neighbour(n2, n1, new_n1);
	replace_neighbour(n2, n3, new_n2);
	replace_neighbour(n3, n2, new_n2);
	replace_neighbour(n3, n1, new_n3);
	replace_neighbour(n1, n3, new_n3);

	give_four_neighbours(new_n1, n2, new_n2, new_n3, n1);
	give_four_neighbours(new_n2, n3, new_n3, new_n1, n2);
	give_four_neighbours(new_n3, n1, new_n1, new_n2, n3);

      return;

END

/*
 *
Function
	add_two_nodes

Function add_two_nodes takes four pointers to nodes and, assuming the
first three originally formed a triangle and the fourth node bisects
one of the sides (the one between the first 2 nodes), creates two new
nodes placed at the center of the non-bisected sides. The function
sets up the neighbour lists of all six nodes accordingly.

The two new nodes created are marked as NEW.  It calls the functions
replace_neighbour and give_four_neighbours and so the function might
cause the program to exit, if the given nodes do not form a triangle.

Arguments: pointers to four nodes
Returns: void
 *
 */

void add_two_nodes(node *n1, node *n2, node *n3, node *new_n1)
BEGIN
	node *new_n2, *new_n3;

	new_n2 = new_node();
	new_n3 = new_node();

	mark(new_n2) = NEW;
	mark(new_n3) = NEW;


/* 6/27/00: takes care of "old" triangle vertices. */
	replace_neighbour(n2, n3, new_n2);
	replace_neighbour(n3, n2, new_n2);
	replace_neighbour(n3, n1, new_n3);
	replace_neighbour(n1, n3, new_n3);


/* 6/27/00: takes care of two "new" midpoint nodes. */
	give_four_neighbours(new_n2, n3, new_n3, new_n1, n2);
	give_four_neighbours(new_n3, n1, new_n1, new_n2, n3);

/* 6/27/00: update the original midpoint node. */

        if(new_n1->degree==5)
        {
           replace_neighbour(new_n1, n3, new_n3);
           add_one_neighbour(new_n1, n2, new_n2);
           changemark(new_n1)=DEGCHANGE;
	}

/* hopefully below exclusively means that it is new  (hasn't been through add_missing_edges) */

        else
        {
           add_neighbours(new_n1, n2, new_n2, new_n3);
	}


     return;
END

/*
 *
Function
	add_one_node

Function add_one_node takes five pointers to nodes and, assuming the
first three originally formed a triangle and the fourth and fifth nodes
bisect two of the sides, creates one new nodes placed at the center of
the non-bisected side. The function sets up the neighbour lists of all
six nodes accordingly.

The new node created is marked as NEW.

It calls the functions replace_neighbour and give_four_neighbours and
so the function might cause the program to exit, if the given nodes do
not form a triangle.

Arguments: pointers to five nodes
Returns: void
 *
 */

void add_one_node(node *n1, node *n2, node *n3, node *new_n1, node *new_n2)
BEGIN
	node *new_n3;

	new_n3 = new_node();

	mark(new_n3) = NEW;


/* new midpoint */
	give_four_neighbours(new_n3, n1, new_n1, new_n2, n3);

/* old vertices */
	replace_neighbour(n1, n3, new_n3);
	replace_neighbour(n3, n1, new_n3);


/* old midpoints */

        if(new_n1->degree==5)
        {
           replace_neighbour(new_n1, n3, new_n3);
           add_one_neighbour(new_n1, n2, new_n2);
           changemark(new_n1)=DEGCHANGE;
	}
        else
        {
           add_neighbours(new_n1, n2, new_n2, new_n3);
	}

        if(new_n2->degree==5)
        {
           replace_neighbour(new_n2, n1, new_n1);
           add_one_neighbour(new_n2, n3, new_n3);
           changemark(new_n2)=DEGCHANGE;
	}
        else
        {
           add_neighbours(new_n2, n3, new_n3, new_n1);
	}


      return;

END

/*
 *
Function
	add_one_triangle

Function add_one_triangle takes 6 pointers to nodes, and updates,
assuming the first three form a triangle in the original mesh,
and the second three bisect the sides formed by the original nodes,
the neighbour list of the three new nodes.

Calls add_neighbours and so could cause the program to EXIT.

Arguments: pointers to six nodes
Returns: void
 *
 */
void add_one_triangle(node *n1, node *n2, node *n3, node *new_n1, node *new_n2, node *new_n3)
BEGIN

/* old midpoints */

        if(new_n1->degree==5)
        {
           replace_neighbour(new_n1, n3, new_n3);
           add_one_neighbour(new_n1, n2, new_n2);
           changemark(new_n1)=DEGCHANGE;
	}
        else
        {
           add_neighbours(new_n1, n2, new_n2, new_n3);
	}


        if(new_n2->degree==5)
        {
           replace_neighbour(new_n2, n1, new_n1);
           add_one_neighbour(new_n2, n3, new_n3);
           changemark(new_n2)=DEGCHANGE;
	}

        else
        {
           add_neighbours(new_n2, n3, new_n3, new_n1);
	}

        if(new_n3->degree==5)
        {
           replace_neighbour(new_n3, n2, new_n2);
           add_one_neighbour(new_n3, n1, new_n1);
           changemark(new_n3)=DEGCHANGE;
	}

        else
        {
           add_neighbours(new_n3, n1, new_n1, new_n2);
	}

      return;


END

/* Function

      check_if_neighbours

      Looks at all neighbours of node n (node at "head" of directional connection), returns True
      if the "tail" (prev_face_node) of the directional connection is in its neighbour list,
      False if the "tail" isn't in its neighbour list.

*/

short check_if_neighbours(node *n, node *prev_face_node)
BEGIN

   node_list *nl;


   nl = n->nl;

   while(nl != NULL)
   {
      if(nl->entry == prev_face_node) return TRUE;
      nl=nl->next;
   }

   return FALSE;

END





/*
 *
Function
	refine_for_node_with_neighbour

Function refine_for_node_with_neighbour takes a pointer to a node "n"
and a pointer "neighbour" to one of its neighbours. This function forms
the core of the refinement method.  It refines the triangle formed by
the node and its neighbour and the node to the right of that neighbour
(viewed from node "n") if that is necessary. The function determines
this necessity by compiling a list of the nodes lying on the "face"
given by the nodes "n"  and "neighbour" in clockwise orientation: the
node "n" is to the left of node "neighbour" in just one face.

New nodes may be created and then are marked as NEW.

It calls functions add_three_nodes, add_two_nodes, add_one_node and
add_one_triangle according to the case determined. So it may cause the
function to EXIT, if the neighbourhood lists are not as expected.

IMPORTANT: the neighbour lists are updated as new triangles are added
around each marked node.  This leaves out the edges according to the
illustration of case 3, of function extend_markings. See function
add_missing_edges.

Arguments: two pointers to node Returns: void

 *
 */



void refine_for_node_with_neighbour(node *n, node *neighbour)
BEGIN
	node *face[5];
	short n_counter;
	node *face_node, *prev_face_node;

	IF (n->fixed && neighbour->fixed && (node_to_the_left(neighbour,n))->fixed) THEN
		return;
	FI
	n_counter = 0;
	prev_face_node = n;
	face_node = neighbour;


	/*
	 *
	Compile list of the face.
 	Count how many nodes lie on the face
	 *
 	 */
	WHILE (face_node != n) DO

              if(changemark(face_node)==DEGCHANGE)
              {
	        printf("kicked out of refnodeneigh %d",n->id);
                return;
	      }

              else
              {
  	        node *nnn;
		nnn = face_node;
		face[n_counter++] = face_node;

		face_node = node_to_the_left(face_node, prev_face_node);

		prev_face_node = nnn;

                if(prev_face_node->degree==5)
                {
		   if(check_if_neighbours(face_node,prev_face_node)==FALSE)
                   {
                      face_node = node_to_the_left(prev_face_node,face_node);
                   }
                   else
                   {
                      return;
                   }
                }
	      }
	OD


	/*
 	 *
	n_counter says how many nodes there are on the face MINUS ONE!
	 *
	 */
	/* Now we have four cases */



	switch(n_counter) {
	case	2:	/*
			 *
			We are looking at a triangle which has not been
			refined. Refine it, if the two nodes are not
			marked as NEW.

			If one of them is marked as NEW, need not take action
			here.


			 *
			 */


			IF (! ((mark(face[0]) == NEW || face[0]->degree==5)
                              && (mark(face[1])== NEW || face[1]->degree==5))) THEN
				add_three_nodes(n, face[0], face[1]);
			FI
			break;

	case	3:
	  /*	     Have four nodes on the face. This means one of
			 them is new, and was added by refining a
			 neighbouring triangle.  We need to add two
			 nodes for the present triangle.

                         6/27/00 Must now also consider that the 4th node is old, and has
                                  degree 5. Two new nodes must be added, a new triangle
                                  created between the 4th node and the new ones, and
                                  the connection between the 4th node and an (old) vertex
                                  node be removed.  */



			 if(mark(face[0]) == NEW || face[0]->degree==5)
			 {
		           if(! ((mark(face[1]) == NEW || face[1]->degree==5)
                              && (mark(face[2])== NEW || face[2]->degree==5)))
			   {

				add_two_nodes(n, face[1], face[2], face[0]);
                           }
                           else break;
			 }
			 else if(mark(face[1]) == NEW || face[1]->degree==5)
			 {
		           if(! ((mark(face[0]) == NEW || face[0]->degree==5)
                              && (mark(face[2])== NEW || face[2]->degree==5)))
			   {

				add_two_nodes(face[0], face[2], n, face[1]);
			   }
                           else break;
			 }
			else
			{
		           if(! ((mark(face[0]) == NEW || face[0]->degree==5)
                              && (mark(face[1])== NEW || face[1]->degree==5)))
			   {

                          add_two_nodes(face[1], n, face[0], face[2]);
			   }
                           else break;
			}
			break;

	case	4:	/*
			 *
			A face with five nodes. Two neighbouring triangles
			have been refined. We need to add one node for the
			present triangle. Determine which are the new
			nodes.

                        6/27/00: must also now consider that the 4th and 5th nodes
                                 could be old, with degree 5. One new node needs to
                                 be added, a new triangle of neighbours made, and
                                 the two connections between the 4th and 5th nodes
                                 and the opposite (old) vertices, must be removed.
       			 *
			 */


			IF (mark(face[0])== NEW || face[0]->degree==5) THEN
			   IF (mark(face[2])== NEW || face[2]->degree==5) THEN
		              if(! ((mark(face[1]) == NEW || face[1]->degree==5)
                                 && (mark(face[3])== NEW || face[3]->degree==5)))
			      {

					add_one_node(n, face[1], face[3], face[0], face[2]);
                              }
                              else break;

			   ELSE
		              if(! ((mark(face[1]) == NEW || face[1]->degree==5)
                                 && (mark(face[2])== NEW || face[2]->degree==5)))
			      {

					add_one_node(face[2], n, face[1], face[3], face[0]);
			      }
                              else break;
			   FI
			ELSE
		           if(! ((mark(face[0]) == NEW || face[0]->degree==5)
                              && (mark(face[2])== NEW || face[2]->degree==5)))
			   {

					add_one_node(face[0], face[2], n, face[1], face[3]);
			   }
                           else break;
			FI

			break;

	case	5:	/*
			 *
			Have six nodes in the face. Should make a
			triangle.

			 *
 			 */

		        if(! ((mark(face[1]) == NEW || face[1]->degree==5)
                           && (mark(face[3])== NEW || face[3]->degree==5)))
		        {
   			  add_one_triangle(n, face[1], face[3], face[0], face[2], face[4]);
			}
			break;

	default:	fprintf(stderr, "SRD_Growth[%d]: Error in face cases while refining!\n", pid);
			EXIT;

	}

     return;

END


/*
 *
Function
	refine_the_node

Function refine_the_node calls the function refine_for_node_with_neighbour
for each of the nodes neighbours.

Argument: pointer to node
Returns: void
 *
 */

void refine_the_node(node *n)
BEGIN
	node_list *nl;
        int i;

	nl = n->nl;
	WHILE(nl != NULL) DO
		refine_for_node_with_neighbour(n, nl->entry);
		nl = nl->next;
	OD

      return;

END

/*
 *
Function
 	refine_all_marked_nodes

Function refine_all_marked_nodes calls refine_the_node for all
nodes which are marked.

Arguments: none
Returns: void
 *
 */
void refine_all_marked_nodes()
BEGIN
	node_list *nl,  *saved_last_nl;
	node *nd;

	nl = first_nl;
	saved_last_nl = last_nl;

	/*
	 *
	New nodes are added to the node-list by the refinement
	procedure. We only refine for those nodes that are from
	the original node-list.
	 *
	 */
	WHILE (nl != saved_last_nl) DO
		node *n = nl->entry;
		IF (mark(n) == MARKED) THEN
		/*                        printf("refining for marked node %d\n",n->id); */
			refine_the_node(n);
		FI
		nl = nl -> next;
	OD
	/*
	 *
	Above loop has not looked at the node which is the
	last in the original node-list.
	 *
	 */
	nd = nl->entry;
	IF (mark(nd) == MARKED) THEN
		refine_the_node(nd);
	FI



      return;

END

/*
 *
Function
	add_one_neighbour

Function add_one_neighbour takes three node pointers and inserts the
third node as a new neighbour in the neighbour list of the first node
"after" the second node.

It marks the node as CHANGED.

Arguments: Three pointers to node
Returns: void
 *
 */

void add_one_neighbour(node *n, node *node_after, node *new_node)
BEGIN
	node_list *nl;
	node_list *new_nl;
        int i;

	nl = n->nl;

	if(changemark(n) != DEGCHANGE) changemark(n) = CHANGED;

	WHILE(nl -> entry != node_after) DO
		nl = nl ->next;
	OD

	new_nl = new_node_list();
	new_nl -> entry = new_node;
	new_nl -> next = nl -> next;
	nl -> next = new_nl;

        set_degree_for_node(n);

      return;
END


/*
 *
 Function
	add_one_edge

Function add_one_edge takes three node pointers and adds an edge
between the node in between (on old triangle) the second and third argument nodes
and the first node argument. (6/27/00: this is Case 3 of extend_markings. Presently
trying to convert this to add only a one-way edge between the new point and the
old opposite vertex. Then, need a function to add a triangle, and remove this
one-way edge, if enough growth has occurred in the "old" triangle.

Arguments: Pointers to three nodes.
Returns: void
 *
 */
void add_one_edge(node *n, node *old_n1, node *old_n2)
BEGIN
	node *old_n0;

/*	printf("in add_one_edge for %d, old_n1 %d (changemark %d),old_n2 %d (changemark %d)\n",n->id,old_n1->id,changemark(old_n1),old_n2->id,changemark(old_n2));  */

	IF (old_n1->fixed && old_n2->fixed) THEN
		n->fixed = TRUE;

/* 8/21/00  need to think about whether this above "fixed" clause is correct. */

	ELSE

	/* Default, set old_n0, reset if not Case 1, 2 in 8/2/00 notes. */

	/*	     printf("Case 1 or 2\n");  */

             old_n0 = node_to_the_left(old_n2,n);

	/* Case 3  */

          if(old_n0->degree==4 && mark(old_n0)==NEW)
          {
	    /*	                 printf("Case 3\n");  */
             old_n0 = node_to_the_left(old_n0,old_n2);
          }

	 /* Case 4  */

         if(old_n0->degree==5 && check_if_neighbours(old_n0,old_n1)==TRUE && check_if_neighbours(old_n1,old_n0)==FALSE)
                {
		  /*		  		  printf("%d\n",4);   */
	           old_n0 = node_to_the_left(old_n0,old_n1);
                }

	/*	removed, 6/27/00	add_one_neighbour(old_n0, old_n1, n); */

	   add_one_neighbour(n, old_n2, old_n0);


	FI
      return;


END


/*
 *
Function
	check_missing_edge

Function check_missing_edge looks at the neighbourhood of the node given
and checks if there are less than three new nodes in its
neighbourhood.  If so a new edge is added to the neighbour lists: an
edge to the old node which forms a triangle with the two old nodes that
are in the nodes neighbourhoods.

This edge is the one indicated in the illustration to Case 3 of
extend_markings(): the edge from "n" to "C"

Such edges are not taken care of in the rest of the refinement
code.

NOTE: The node given is assumed to be a NEW node.

The function causes the program to EXIT if more that two old
neighbours are found.

Argument: pointer to node
Returns: void
 *
 */

void check_for_missing_edge(node *n)
BEGIN
	node *old_nodes[4];
	node_list *nl;
        node *temp;
        int i,j;

	nl = n->nl;

	for(i=0;i<4;i++)
        {
       	     old_nodes[i] = nl->entry;
	     nl = nl->next;
	}

/* "Bubble sort" should put lowest id nodes into positions 1 and 2 of old_nodes. */




        for(i=1;i<3;i++)
        {
           for(j=0;j<3;j++)
	   {
              if(old_nodes[j]->id > old_nodes[j+1]->id)
	      {
                 temp = old_nodes[j];
                 old_nodes[j] = old_nodes[j+1];
                 old_nodes[j+1] = temp;
	      }
	   }
	}


/* from here, assumes old_nodes[0] lowest id, old_nodes[1] 2nd lowest id*/

        if(node_to_the_left(n,old_nodes[0])->id == old_nodes[1]->id)
        {
	     add_one_edge(n, old_nodes[0], old_nodes[1]);
	}

        else
        {
             add_one_edge(n,old_nodes[1],old_nodes[0]);
	}






/*		if((mark(nb)!= NEW && changemark(nb) != DEGCHANGE) && (mark(node_to_the_left(n,nb))!=NEW && changemark(node_to_the_left(n,nb)) != DEGCHANGE)) */

      return;

END

/*
 *
Function
	add_missing_edges

Function add_missing_edges calls
check_missing_edges for all NEW edges.

Arguments: none
Returns: void
 *
 */
void add_missing_edges()
BEGIN
	node_list *nl;
	nl = first_nl;

	WHILE (nl != NULL) DO
		node *n = nl->entry;
		IF (n->degree==4 && mark(n)==NEW) THEN
			check_for_missing_edge(n);
		FI
		nl = nl->next;
	OD

      return;

END

/*
 *
Function
	set_values_for_node

Function set_values_for_node takes a node pointer and sets the x,y,z
coordinates and the X,Y values as the arithmetic mean of those values
at the neighbours which lie on the original mesh. It also sets the
degree (NO).

Arguments: one node pointer
Returns: void
 *
 */
void set_values_for_node(node *n)
BEGIN

        FILE *save;
	node_list *nl;
	double sum_x = 0;
	double sum_y = 0;
	double sum_z = 0;

	double sum_X = 0;
	double sum_Y = 0;
	double sum_A = 0;

	short n_count = 0;
/*	short degree = 0; */

	nl = n->nl;
	WHILE (nl != NULL) DO
		node *nb = nl -> entry;

/*		degree ++; */
		IF (mark(nb) != NEW) THEN
			sum_x += nb->x;
			sum_y += nb->y;
			sum_z += nb->z;

			sum_X += nb->X;
			sum_Y += nb->Y;
			sum_A += nb->A;
			n_count ++;
		FI
		nl = nl->next;
	OD
	/*	n->degree = degree; */

	n->x = sum_x / n_count;
	n->y = sum_y / n_count;
	n->z = sum_z / n_count;

	n->X = sum_X / n_count;
	n->Y = sum_Y / n_count;
	n->A = sum_A / n_count;


/*        save = fopen("./ids.dat","a");

	       fprintf(save,"%d,%d,%d\n",n->id,n->nl->entry->id,n->degree);
	       fprintf(save,"%d,%d\n",n->id,n->nl->next->entry->id);
	       fprintf(save,"%d,%d\n",n->id,n->nl->next->next->entry->id);
	       fprintf(save,"%d,%d\n",n->id,n->nl->next->next->next->entry->id);
	       fprintf(save,"%d,%d\n",n->id,n->nl->next->next->next->next->entry->id);
	       fprintf(save,"%d,%d\n",n->id,n->nl->next->next->next->next->next->entry->id);

	       fclose(save);        */

      return;

END

/*
 *
Function
	set_degree_for_node

Function set_degree_for_node calculates the number of neighbours
of a (new) node and sets the entry in the degree-array accordingly.

Arguments: one pointer to node
Returns: void
 *
 */
void set_degree_for_node(node *n)
BEGIN
	node_list *nl;
	int degree = 0;
	nl = n->nl;
	WHILE(nl!= NULL) DO
		degree ++;
		nl = nl->next;
	OD
	n->degree = degree;


      return;


END

/*
 *
Function
	set_new_values_for_nodes


Function set_new_values_for_nodes calls
	set_values_for_node for all NEW nodes
and
	set_degree_for_node
for all nodes which are marked CHANGED (old - set_degree_for_node now called directly
                                         after changes)

Arguments: none
Returns: void
 *
 */

void set_new_values_for_nodes()
BEGIN
	node_list *nl;
	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		IF (mark(n) == NEW) THEN
			set_values_for_node(n);
/*		ELIF (changemark(n) == CHANGED || changemark(n)==DEGCHANGE) THEN
		set_degree_for_node(n); */
		FI
		nl = nl -> next;
	OD
      return;


END

/*
 *
Function
	allocate_matrix_arrays_for_node

Function allocate_matrix_arrays_for_node takes one node pointer argument
and allocates entries in the four arrays Lapl,Area,MatrixX,MatrixY

New nodes get as many such entries as their degree specifies, old nodes
might have been given new neighbours and need a few more entries.

Arguments: pointer to node
Returns: void
 *
 */
void allocate_matrix_arrays_for_node(node *n)
BEGIN
	coeff_list *cl_Lapl, *cl_Area, *cl_MatrixX, *cl_MatrixY, *cl_MatrixA;
	node_list *nl;
	int i;
	int required;
	int degree;

	degree = 0;
	nl = n->nl;
	WHILE (nl != NULL) DO
		degree ++;
		nl = nl->next;
	OD
	n->degree = degree;

	required = degree;

	cl_Lapl = n->Lapl_coeff_l;
	WHILE(cl_Lapl != NULL) DO
		required --;
		cl_Lapl = cl_Lapl -> next;
	OD

	IF (required < 1) THEN
		return;
	FI

	FOR (i=0; i<required; i++) DO
		cl_Lapl = new_coeff_list();
		cl_Area = new_coeff_list();
		cl_MatrixX = new_coeff_list();
		cl_MatrixY = new_coeff_list();
		cl_MatrixA = new_coeff_list();

		cl_Lapl -> next = n->Lapl_coeff_l;
		cl_Area -> next = n->Area_coeff_l;
		cl_MatrixX -> next = n->Matrix_coeff_l_X;
		cl_MatrixY -> next = n->Matrix_coeff_l_Y;
		cl_MatrixA -> next = n->Matrix_coeff_l_A;

		n->Lapl_coeff_l = cl_Lapl;
		n->Area_coeff_l = cl_Area;
		n->Matrix_coeff_l_X = cl_MatrixX;
		n->Matrix_coeff_l_Y = cl_MatrixY;
		n->Matrix_coeff_l_A = cl_MatrixA;
	OD

	IF (mark(n) == NEW) THEN
		allocate_imex_arrays_for_node(n);
	FI

      return;

END

/*
 *
Function
	set_matrix_arrays_for_changed_nodes

Function set_matrix_arrays_for_changed_nodes calls

	allocate_matrix_arrays_for_node
        set_coefficients_for_node
        set_for_node_RK_matrix_coefficients_0

for each node that is marked CHANGED, or NEW.

The function set_coefficients_for_node is from setup_matrices and
sets the Area and Lapl matrix entries.

Arguments: none
Returns: void
 *
 */
void set_matrix_arrays_for_changed_nodes()
BEGIN
	node_list *nl;
        int j = 0;


	nl = first_nl;

	WHILE(nl != NULL) DO
		node *n = nl->entry;

		IF (changemark(n) == CHANGED || changemark(n) == DEGCHANGE || mark(n) == NEW) THEN
			allocate_matrix_arrays_for_node(n);
			set_coefficients_for_node(n);
/*                         set_RK_matrix_coefficients(0); */
			set_for_node_RK_matrix_coefficients_0(n);
		FI
		nl = nl ->next;
	OD
      return;

END

/*
Function
	refine_if_necessary

Function refine_if_necessary first checks whether any triangles need to
be refined, and if so refines around the nodes which are marked so by
the function mark_nodes. Then missing edges according to the
illustration of Case 3 of extend_markings are added.

The Area/Lapl matrices are initialized for the Runge-Kutta method.  The
arrays for the Runge-Kutta method are allocated.

Arguments: none
Returns: void
 *
 */

short refine_if_necessary()
BEGIN
        node_list *nl;
        int i;
        FILE *save2;

	IF (!mark_nodes()) THEN
		return(FALSE);
	FI
fprintf(stderr, "SRD_Growth[%d]: Refining the mesh ... length_tolerance = %f\n", pid, length_tolerance);
fprintf(stderr, "Refining marked nodes\n");



	refine_all_marked_nodes();

       IF (user_stops_exe == TRUE) THEN
          printf("aft ref mar nod, user_stops_exe is TRUE\n");
       FI


fprintf(stderr, "Adding missing edges    \n");
   add_missing_edges();


fprintf(stderr, "Setting values for new nodes\n");
	set_new_values_for_nodes();


fprintf(stderr, "Allocating matrix arrays.    \n");
	set_matrix_arrays_for_changed_nodes();

/*  ids.dat fprintf here */
	    save2 = fopen("./ids.dat","w");
            nl = first_nl;
            while(nl!=NULL)
	    {
   	       node *n=nl->entry;

	       fprintf(save2,"%d,%d\n",n->id,n->degree);
	       /*	       fprintf(save2,"%d,%d\n",n->id,n->nl->next->entry->id);
	       fprintf(save2,"%d,%d\n",n->id,n->nl->next->next->entry->id);
	       fprintf(save2,"%d,%d\n",n->id,n->nl->next->next->next->entry->id);
	       fprintf(save2,"%d,%d\n",n->id,n->nl->next->next->next->next->entry->id);
	       fprintf(save2,"%d,%d\n",n->id,n->nl->next->next->next->next->next->entry->id); */

	       nl = nl->next;
	    }
            fclose(save2); 

	count_node_degrees();

	fprintf(stderr, "SRD_Growth[%d]: Refining done!\n",pid);

	return(TRUE);
END
