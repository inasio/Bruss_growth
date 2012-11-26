
#ifdef USE_TRIANGLES
extern REAL triangle_area(triangle *);
extern REAL X_of_triangle(triangle *);
extern REAL Y_of_triangle(triangle *);
extern void set_normal_vector(triangle *);
extern void give_triangle(triangle *);
#endif

extern REAL area_of_triangle(node *, node *, node *);
extern void count_node_degrees();

#ifdef USE_TRIANGLES
extern REAL abs_error_of_triangle(triangle *);
#endif
