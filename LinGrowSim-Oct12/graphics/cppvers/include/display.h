//#include "types.h"
/*  #include "BM.h"*/
//#include "globals.h"

//extern void newdisplay(int, char**);
//extern void init_display();
//extern void display();

extern void init_display(int, char**);
extern void init_triangles();
extern void display_triangles();
//extern void display_color_mapped_triangles(float**);
extern void display_grid(void);
extern void check_display();
extern void interpret_users_inputs(void);
extern void wait_user_spaces(node *);  
extern void display_node_with_neighbours(node *nd);  
#ifdef USE_TRIANGLES
extern void display_triangles_with_neighbours(triangle *nd);
#endif
extern void count_node_degrees(void);

extern void set_initial_scale();
 
extern void display_previous_picture();
extern void display_next_picture();
extern void print_picture();
extern void jump_to_numbered_picture(int);
extern void redraw_window();
extern void give_color_bar(float (*)[3]);
extern void give_text_and_bar();
extern void draw_axes();

extern short stop_drawing;
extern float scale_value;
extern float rotate_x, rotate_y, rotate_z; 

extern short   mode;
extern short show_grid;

extern short use_users_ranges;
extern REAL user_max_X, user_min_X, user_max_Y, user_min_Y;
//int picturecounter; 
