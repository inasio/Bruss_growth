/*********************************** functions depending on X or Y *****************************/
void set_norm_b()
BEGIN
	node_list *nl;
	double sum = 0.0;
	nl=first_nl;
	WHILE(nl != NULL) DO
		double bbb =nl->entry->rhs;
		sum += bbb*bbb;
		nl=nl->next;
	OD
	norm_b = sqrt(sum);
	IF (norm_b > 1.0) THEN
		norm_b = 1/norm_b;
	FI
END

void set_rpx_initially()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;

		n->solution = 0.0;
		n->BiCGSTAB_r =n->rhs;
		n->BiCGSTAB_p = n->BiCGSTAB_r;

		nl = nl->next;
	OD
END


void 	set_rho_to_bTr()
BEGIN
	node_list *nl;

	rho = 0.0;
	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;

		rho += 	n->rhs * n->BiCGSTAB_r;
		nl = nl->next;
	OD
END

void  set_v_to_A_phat()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		double value;

		coeff_list *cl = n->Matrix_coeff_l;
		node_list *nbl = n->nl;

		value = n->Matrix_coeff * n->BiCGSTAB_phat;
		WHILE (nbl != NULL) DO
			node *nb = nbl->entry;
			value += cl -> entry  * nb->BiCGSTAB_phat;

			cl = cl->next;
			nbl = nbl -> next;
		OD

		n -> BiCGSTAB_v = value;

		nl = nl->next;
	OD
END

void set_alpha_to_rho_over_bTv()
BEGIN
	node_list *nl;
	double bTv;

	nl = first_nl;
	bTv = 0.0;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		bTv += n->rhs * n->BiCGSTAB_v;
		nl = nl->next;
	OD
	alpha = rho / bTv;
END

#define UNSETVALUE  1.234001e9
void set_shat_MGSs()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		nl->entry->BiCGSTAB_shat = UNSETVALUE;

		nl = nl->next;
	OD

	nl = first_nl;
	WHILE(nl != NULL) DO
		node *n = nl->entry;
		/* determine coefficients for equation Matrix_coeff * x + bn = sn */
		double bn;

		coeff_list *cl = n->Matrix_coeff_l;
		node_list *nbl = n->nl;

		bn = 0.0;
		WHILE (nbl != NULL) DO
			node *nb = nbl->entry;
			IF (nb -> BiCGSTAB_shat != UNSETVALUE) THEN
				bn += cl -> entry * nb -> BiCGSTAB_shat;
			FI

			cl = cl->next;
			nbl = nbl -> next;
		OD

		n -> BiCGSTAB_shat = (n->BiCGSTAB_s - bn) / n->Matrix_coeff;

		nl = nl->next;
	OD
END

#ifndef UNSETVALUE
#define UNSETVALUE  1.234001e9
#endif
void set_phat_MGSp()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		nl->entry->BiCGSTAB_phat = UNSETVALUE;

		nl = nl->next;
	OD

	nl = first_nl;
	WHILE(nl != NULL) DO
		node *n = nl->entry;
		/* determine coefficients for equation Matrix_coeff * x + bn = pn */
		double bn;

		coeff_list *cl = n->Matrix_coeff_l;
		node_list *nbl = n->nl;

		bn = 0.0;
		WHILE (nbl != NULL) DO
			node *nb = nbl->entry;
			IF (nb -> BiCGSTAB_phat != UNSETVALUE) THEN
				bn += cl -> entry * nb -> BiCGSTAB_phat;
			FI

			cl = cl->next;
			nbl = nbl -> next;
		OD

		n -> BiCGSTAB_phat = (n->BiCGSTAB_p - bn) / n->Matrix_coeff;

		nl = nl->next;
	OD
END


short BiCGSTABconverged()
BEGIN
	node_list *nl;
	double norm_Ax_minus_b;
	nl = first_nl;
	norm_Ax_minus_b = 0.0;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		double value;

		coeff_list *cl = n->Matrix_coeff_l;
		node_list *nbl = n->nl;

		value = n->Matrix_coeff * n->solution;
		WHILE (nbl != NULL) DO
			node *nb = nbl->entry;
			value += cl->entry * nb -> solution;

			cl = cl->next;
			nbl = nbl -> next;
		OD
		value = value - n->rhs;

		norm_Ax_minus_b += value * value;
		nl = nl->next;
	OD
#ifdef TRACE_CONVERGENCE
	/*fprintf(stderr, "\rNorm of A*x-b is at %2.12f    ", norm_Ax_minus_b);*/
#endif

	norm_Ax_minus_b = sqrt(norm_Ax_minus_b) / norm_b;

	IF (norm_Ax_minus_b < 1e-12) THEN

#ifdef TRACE_CONVERGENCE
	/*fprintf(stderr, "\rNorm Ax-b/Norm b=%g", norm_Ax_minus_b); */
#endif
		return TRUE;
	ELSE

		return FALSE;
	FI
END


void setx_to_x_plus_alpha_phat()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;

		n -> solution +=  alpha * n->BiCGSTAB_phat;
		nl = nl->next;
	OD
END


void set_t_to_A_shat()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;
		double value;

		coeff_list *cl = n->Matrix_coeff_l;
		node_list *nbl = n->nl;

		value = n->Matrix_coeff * n->BiCGSTAB_shat;
		WHILE (nbl != NULL) DO
			node *nb = nbl->entry;
			value += cl->entry  * nb -> BiCGSTAB_shat;

			cl = cl->next;
			nbl = nbl -> next;
		OD

		n -> BiCGSTAB_t = value;

		nl = nl->next;
	OD
END


void set_x_to_x_plus_alpha_phat_plus_omega_shat()
BEGIN
	node_list *nl;

	nl = first_nl;
	WHILE (nl != NULL) DO
		node *n = nl->entry;

		n->solution += alpha * n->BiCGSTAB_phat +
			       omega * n->BiCGSTAB_shat;
		nl = nl->next;
	OD
END
/******************************* end functions depending on X or Y ****************************************/





