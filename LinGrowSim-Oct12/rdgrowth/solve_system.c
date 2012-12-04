#include "BM.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "types.h"
#define GLOBAL extern
#include "globals.h"
#include "memory.h"
#include "triangle.h"
#include "display.h"

#define GIVE_ITERATIONS
#define TRACE_CONVERGENCE

double alpha, beta, rho, prev_rho, omega;
double norm_b;

/**************************** Functions for X ************************************/
#define solution	solution_X
#define rhs		rhs_X
#define Matrix_coeff	Matrix_coeff_X
#define Matrix_coeff_l	Matrix_coeff_l_X

#define set_norm_b					X_set_norm_b
#define set_rpx_initially				X_set_rpx_initially
#define set_rho_to_bTr					X_set_rho_to_bTr
#define set_v_to_A_phat					X_set_v_to_A_phat
#define set_alpha_to_rho_over_bTv			X_set_alpha_to_rho_over_bTv
#define set_shat_MGSs					X_set_shat_MGSs
#define set_phat_MGSp					X_set_phat_MGSp
#define BiCGSTABconverged				X_BiCGSTABconverged
#define setx_to_x_plus_alpha_phat			X_setx_to_x_plus_alpha_phat
#define set_t_to_A_shat					X_set_t_to_A_shat
#define set_x_to_x_plus_alpha_phat_plus_omega_shat	X_set_x_to_x_plus_alpha_phat_plus_omega_shat

#include "solve_system_DEPXY.c"
/******************************* end functions for X ****************************************/
#undef solution
#undef rhs
#undef Matrix_coeff
#undef Matrix_coeff_l
#undef set_norm_b
#undef set_rpx_initially
#undef set_rho_to_bTr
#undef set_v_to_A_phat
#undef set_alpha_to_rho_over_bTv
#undef set_shat_MGSs
#undef set_phat_MGSp
#undef BiCGSTABconverged
#undef setx_to_x_plus_alpha_phat
#undef set_t_to_A_shat
#undef set_x_to_x_plus_alpha_phat_plus_omega_shat
/**************************** Functions for Y ************************************/
#define solution	solution_Y
#define rhs		rhs_Y
#define Matrix_coeff	Matrix_coeff_Y
#define Matrix_coeff_l	Matrix_coeff_l_Y

#define set_norm_b					Y_set_norm_b
#define set_rpx_initially				Y_set_rpx_initially
#define set_rho_to_bTr					Y_set_rho_to_bTr
#define set_v_to_A_phat					Y_set_v_to_A_phat
#define set_alpha_to_rho_over_bTv			Y_set_alpha_to_rho_over_bTv
#define set_shat_MGSs					Y_set_shat_MGSs
#define set_phat_MGSp					Y_set_phat_MGSp
#define BiCGSTABconverged				Y_BiCGSTABconverged
#define setx_to_x_plus_alpha_phat			Y_setx_to_x_plus_alpha_phat
#define set_t_to_A_shat					Y_set_t_to_A_shat
#define set_x_to_x_plus_alpha_phat_plus_omega_shat	Y_set_x_to_x_plus_alpha_phat_plus_omega_shat

#include "solve_system_DEPXY.c"
/******************************* end functions for Y ****************************************/

#undef solution
#undef rhs
#undef Matrix_coeff
#undef Matrix_coeff_l
#undef set_norm_b
#undef set_rpx_initially
#undef set_rho_to_bTr
#undef set_v_to_A_phat
#undef set_alpha_to_rho_over_bTv
#undef set_shat_MGSs
#undef set_phat_MGSp
#undef BiCGSTABconverged
#undef setx_to_x_plus_alpha_phat
#undef set_t_to_A_shat
#undef set_x_to_x_plus_alpha_phat_plus_omega_shat

/**************************** Functions for A ************************************/
#define solution	solution_A
#define rhs		rhs_A
#define Matrix_coeff	Matrix_coeff_A
#define Matrix_coeff_l	Matrix_coeff_l_A

#define set_norm_b					A_set_norm_b
#define set_rpx_initially				A_set_rpx_initially
#define set_rho_to_bTr					A_set_rho_to_bTr
#define set_v_to_A_phat					A_set_v_to_A_phat
#define set_alpha_to_rho_over_bTv			A_set_alpha_to_rho_over_bTv
#define set_shat_MGSs					A_set_shat_MGSs
#define set_phat_MGSp					A_set_phat_MGSp
#define BiCGSTABconverged				A_BiCGSTABconverged
#define setx_to_x_plus_alpha_phat			A_setx_to_x_plus_alpha_phat
#define set_t_to_A_shat					A_set_t_to_A_shat
#define set_x_to_x_plus_alpha_phat_plus_omega_shat	A_set_x_to_x_plus_alpha_phat_plus_omega_shat

#include "solve_system_DEPXY.c"
/******************************* end functions for A ****************************************/

/********************************* end depending on X,Y,A **************************************/

/********************************* begin not depending on X,Y,A *********************************/

void set_p_to_r_plus_beta_p_minus_omega_v()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;

		n->BiCGSTAB_p = n->BiCGSTAB_r + beta * (n->BiCGSTAB_p - omega * n->BiCGSTAB_v);
		nl = nl->next;
	OD
END

void set_omega_to_tTs_over_tTt()
BEGIN
	node_list *nl;
	double tTs, tTt;

	tTs = 0.0;
	tTt = 0.0;
	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;

		tTs += n->BiCGSTAB_t * n->BiCGSTAB_s;
		tTt += n->BiCGSTAB_t * n->BiCGSTAB_t;
		nl = nl->next;
	OD
	omega = tTs / tTt;
END

void set_r_to_s_minus_omega_t()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;

		n->BiCGSTAB_r = n->BiCGSTAB_s - omega * n->BiCGSTAB_t;
		nl = nl->next;
	OD
END


void set_s_to_r_minus_alpha_v()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;

		n -> BiCGSTAB_s = n->BiCGSTAB_r - alpha * n->BiCGSTAB_v;
		nl = nl->next;
	OD
END

double BiCGSTABnorm_s()
BEGIN
	node_list *nl;
	double norm = 0.0;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		norm += n->BiCGSTAB_s * n->BiCGSTAB_s;
		nl = nl->next;
	OD
	return sqrt(norm);
END

/*************************************** end not depending on X or Y ******************************/

void solve_system_X(void)
BEGIN
	long it;
        X_set_rpx_initially();
	X_set_rho_to_bTr();
	X_set_norm_b();


	it = 0;
#ifdef TRACE_CONVERGENCE
/*fprintf(stderr, "\n----------------- X X X-------------\n");   */
#endif
	WHILE (TRUE) DO
		X_set_phat_MGSp();
		X_set_v_to_A_phat();
		X_set_alpha_to_rho_over_bTv();
		set_s_to_r_minus_alpha_v();
		IF (BiCGSTABnorm_s()/norm_b < 1e-14) THEN
			X_setx_to_x_plus_alpha_phat();
			break;
		FI
		X_set_shat_MGSs();
		X_set_t_to_A_shat();
		set_omega_to_tTs_over_tTt();
		X_set_x_to_x_plus_alpha_phat_plus_omega_shat();
		set_r_to_s_minus_omega_t();
		IF (X_BiCGSTABconverged()) THEN
			break;
		FI
		IF (omega == 0.0) THEN
	              	fprintf(stderr, "SRD_Growth[%d]: (X)BiConjugate Gradient solver terminated since omega reached 0!\n", pid);
			EXIT ;
		FI
		prev_rho = rho;
		X_set_rho_to_bTr();
		IF (rho == 0.0) THEN
				fprintf(stderr, "SRD_Growth[%d]: (X)BiConjugate Gradient solver terminated since rho reached 0!\n", pid);
			EXIT  ;
		FI
		beta = rho / prev_rho * alpha / omega;
		set_p_to_r_plus_beta_p_minus_omega_v();

		it ++;

       if(it>1000)
       {
           user_stops_exe = TRUE;
           exestopmsg = "matrix solver couldn't converge";
           return;
       } 

#ifdef TRACE_CONVERGENCE

/*fprintf(stderr, "(X)BiCGSTAB at iteration %d.     \r", it);  */

#endif

	      OD
#ifdef TRACE_CONVERGENCE

	      /*	            fprintf(stderr, "\r");  */
X_BiCGSTABconverged();
/*fprintf(stderr, "(X)converged at It=%d. (Norm b at %f)    \n", it, norm_b);   */

#endif

#ifdef GIVE_ITERATIONS
IF (iterations % 17 == 0) THEN
/*fprintf(stderr, "(X)Time t=%f (Norm b at %f) converged at It=%d.\n",time_start+iterations*delta_t,norm_b, it);  */

FI
#endif

END

void solve_system_Y(void)
BEGIN
	long it;
        Y_set_rpx_initially();
	Y_set_rho_to_bTr();
	Y_set_norm_b();

	it = 0;
#ifdef TRACE_CONVERGENCE
/*fprintf(stderr, "\n----------------- Y Y Y -------------\n"); */
#endif


	WHILE (TRUE) DO
		Y_set_phat_MGSp();
		Y_set_v_to_A_phat();
		Y_set_alpha_to_rho_over_bTv();
		set_s_to_r_minus_alpha_v();
		IF (BiCGSTABnorm_s()/norm_b < 1e-14) THEN
			Y_setx_to_x_plus_alpha_phat();
			break;
		FI
		Y_set_shat_MGSs();
		Y_set_t_to_A_shat();
		set_omega_to_tTs_over_tTt();
		Y_set_x_to_x_plus_alpha_phat_plus_omega_shat();
		set_r_to_s_minus_omega_t();
		IF (Y_BiCGSTABconverged()) THEN
			break;
		FI
		IF (omega == 0.0) THEN
		/*		fprintf(stderr, "SRD_Growth[%d]: (Y)BiConjugate Gradient solver terminated since omega reached 0!\n", pid); */
			EXIT  ;
		FI
		prev_rho = rho;
		Y_set_rho_to_bTr();
		IF (rho == 0.0) THEN
		/*		fprintf(stderr, "SRD_Growth[%d]: (Y)BiConjugate Gradient solver terminated since rho reached 0!\n", pid);  */
			EXIT  ;
		FI
		beta = rho / prev_rho * alpha / omega;
		set_p_to_r_plus_beta_p_minus_omega_v();

		it ++;

       if(it>1000)
       {
           user_stops_exe = TRUE;
           exestopmsg = "matrix solver couldn't converge";
           return;
       } 

#ifdef TRACE_CONVERGENCE

/*fprintf(stderr, "(Y)BiCGSTAB at iteration %d.     \r", it);   */

#endif

	      OD
#ifdef TRACE_CONVERGENCE

	      /*	      fprintf(stderr, "\r"); */
Y_BiCGSTABconverged();
/*fprintf(stderr, "(Y)converged at It=%d. (Norm b at %f)   \n", it, norm_b);  */

#endif

#ifdef GIVE_ITERATIONS
IF (iterations % 17 == 0) THEN
/*fprintf(stderr, "(Y)Time t=%f (Norm b at %f) converged at It=%d.\n",time_start+iterations*delta_t,norm_b, it);  */

FI
#endif

END


void solve_system_A(void)
BEGIN
	long it;
        A_set_rpx_initially();
	A_set_rho_to_bTr();
	A_set_norm_b();

	it = 0;


#ifdef TRACE_CONVERGENCE
/*fprintf(stderr, "\n----------------- A A A -------------\n"); */
#endif

	WHILE (TRUE) DO
		A_set_phat_MGSp();
		A_set_v_to_A_phat();
		A_set_alpha_to_rho_over_bTv();
		set_s_to_r_minus_alpha_v();

		IF (BiCGSTABnorm_s()/norm_b < 1e-14) THEN
			A_setx_to_x_plus_alpha_phat();
			break;
		FI
		A_set_shat_MGSs();
		A_set_t_to_A_shat();
		set_omega_to_tTs_over_tTt();
		A_set_x_to_x_plus_alpha_phat_plus_omega_shat();
		set_r_to_s_minus_omega_t();

		IF (A_BiCGSTABconverged()) THEN
			break;
		FI

		IF (omega == 0.0) THEN
		/*		fprintf(stderr, "SRD_Growth[%d]: (A)BiConjugate Gradient solver terminated since omega reached 0!\n", pid); */
			EXIT  ;
		FI
		prev_rho = rho;
		A_set_rho_to_bTr();
		IF (rho == 0.0) THEN
		/*	      	fprintf(stderr, "SRD_Growth[%d]: (A)BiConjugate Gradient solver terminated since rho reached 0!\n", pid);  */
			EXIT  ;
		FI
		beta = rho / prev_rho * alpha / omega;
		set_p_to_r_plus_beta_p_minus_omega_v();

		it ++;

       if(it>1000)
       {
           user_stops_exe = TRUE;
           exestopmsg = "matrix solver couldn't converge";
           return;
       } 

#ifdef TRACE_CONVERGENCE

/*fprintf(stderr, "(A)BiCGSTAB at iteration %d.     \r", it);   */

#endif

	      OD
	      /*  printf("in solve_systemA, 1st while done\n"); */

#ifdef TRACE_CONVERGENCE

	      /*	      fprintf(stderr, "\r"); */
A_BiCGSTABconverged();
/*fprintf(stderr, "(A)converged at It=%d. (Norm b at %f)   \n", it, norm_b);  */

#endif

#ifdef GIVE_ITERATIONS
IF (iterations % 17 == 0) THEN
/*fprintf(stderr, "(Y)Time t=%f (Norm b at %f) converged at It=%d.\n",time_start+iterations*delta_t,norm_b, it);  */

FI
#endif

END





void solve_system()
BEGIN
/*          printf("begin, solve_systemX\n"); */
	solve_system_X();
/*          printf("done, solve_systemX\n"); */
	solve_system_Y();
/*          printf("done, solve_systemY\n");  */
	solve_system_A();
/*          printf("done, solve_systemA\n");  */

END
