extern void reaction_diffusion(void);
extern void diffusion(void);

extern void intialize_imex_arrays(void);
extern void set_RK_matrix_coefficients(short);


void	allocate_RK_matrix_coefficients_for_node(node *);
void	set_for_node_RK_matrix_coefficients_0(node *);
