
#define REAL double

GLOBAL int border_order,
	   order_points,
	   center_order,
	   order_triangles;

GLOBAL int next_point, next_triangle;

GLOBAL int last_sequence_first;

GLOBAL REAL *x, *y;
GLOBAL int *p, *p_inverse;
GLOBAL int *t_array;

GLOBAL int *node_nl_array;
GLOBAL int *node_tl_array;

GLOBAL int *triangle_nl_array;

GLOBAL int user_stops_exe;

GLOBAL int random_seed;

#define T(i1,i2)  (t_array[3*(i1)+(i2)])

#define MX(n1,n2) ( (n1) > (n2) ? (n1) : (n2) )
#define node_nl(i1,i2) (node_nl_array[MX(6,center_order)*(i1) + (i2)])


#define TRUE 1
#define FALSE 0
