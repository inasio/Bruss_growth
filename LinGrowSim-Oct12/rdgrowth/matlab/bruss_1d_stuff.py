from numpy import sin, cos, sqrt, zeros, random, arange, pi
from scipy import integrate
from pylab import figure, plot, show, axis, ceil
from mayavi.mlab import surf, axes
from scipy import weave

# *** start ipython as ipython --gui=wx ***

# finite difference discretizations for various systems   
def XY2_stationary_explicit(y, t, *args):
	"""Solve explicitly a 1D coupled system of two Brusselators"""
	Dx = args[0]
	Dy = args[1]
	N = args[2]
	H1 = args[3]
	L1 = args[4]
	H2 = args[5]
	L2 = args[6]
	
	F1_params = args[7]
	G1_params = args[8]
	F2_params = args[9]
	G2_params = args[10]
	
	F1_func = F1_params[0]
	args_f1 = F1_params[1]
	G1_func = G1_params[0]
	args_g1 = G1_params[1]
	F2_func = F2_params[0]
	args_f2 = F2_params[1]
	G2_func = G2_params[0]
	args_g2 = G2_params[1]

	dxydt = zeros(4*N)
	xx1 = y[:N]
	yy1 = y[N:2*N]
	xx2 = y[2*N:3*N]
	yy2 = y[3*N:4*N]

	cx1 = Dx/(2*L1*H1)**2
	cy1 = cx1*Dy/Dx
	cx2 = Dx/(2*L2*H2)**2
	cy2 = cx2*Dy/Dx

	dx1dt = zeros(N)
	dy1dt = zeros(N)
	dx2dt = zeros(N)
	dy2dt = zeros(N)
	
	RHS_x1 = zeros(N)
	RHS_y1 = zeros(N)
	RHS_x2 = zeros(N)
	RHS_y2 = zeros(N)
	
	RHS_x1 = F1_func(xx1, yy1, args_f1)
	RHS_y1 = G1_func(xx1, yy1, args_g1)
	RHS_x2 = F2_func(xx2, yy2, [yy1]+args_f2)
	RHS_y2 = G2_func(xx2, yy2, [yy1]+args_g2)

	dx1dt[0] = 2*cx1*(xx1[1] - xx1[0]) + RHS_x1[0]
	dx1dt[N-1] = 2*cx1*(xx1[N-2] - xx1[N-1]) + RHS_x1[N-1]

	dy1dt[0] = 2*cy1*(yy1[1] - yy1[0]) + RHS_y1[0]
	dy1dt[N-1] = 2*cy1*(yy1[N-2] - yy1[N-1]) + RHS_y1[N-1]
	
	dx2dt[0] = 2*cx2*(xx2[1] - xx2[0]) + RHS_x2[0]
	dx2dt[N-1] = 2*cx2*(xx2[N-2] - xx2[N-1]) + RHS_x2[N-1]

	dy2dt[0] = 2*cy2*(yy2[1] - yy2[0]) + RHS_y2[0]
	dy2dt[N-1] = 2*cy2*(yy2[N-2] - yy2[N-1]) + RHS_y2[N-1]	
		
	expr1 = "dx1dt[1:N-1] = cx1*(xx1[:N-2]-2*xx1[1:N-1]+xx1[2:N]) + RHS_x1[1:N-1]"
	expr2 = "dy1dt[1:N-1] = cy1*(yy1[:N-2]-2*yy1[1:N-1]+yy1[2:N]) + RHS_y1[1:N-1]"
	expr3 = "dx2dt[1:N-1] = cx2*(xx2[:N-2]-2*xx2[1:N-1]+xx2[2:N]) + RHS_x2[1:N-1]"
	expr4 = "dy2dt[1:N-1] = cy2*(yy2[:N-2]-2*yy2[1:N-1]+yy2[2:N]) + RHS_y2[1:N-1]"
	weave.blitz(expr1, check_size=0)
	weave.blitz(expr2, check_size=0)
	weave.blitz(expr3, check_size=0)
	weave.blitz(expr4, check_size=0)
		
	for i in range(N):
		dxydt[i] = dx1dt[i]
		dxydt[N+i] = dy1dt[i]
		dxydt[2*N+i] = dx2dt[i]
		dxydt[3*N+i] = dy2dt[i]

	return dxydt

def diffusion_stationary_explicit(y, t, *args):
	"""Solve explicitly a 1D diffusion equation"""
	N = args[0]
	H = args[1]
	L = args[2]
	
	dxdt = zeros(N)
	cx = 1./(L*H)**2
	xx = y[:N]

	dxdt[0] = 2*cx*(xx[1] - xx[0])
	dxdt[N-1] = 2*cx*(xx[N-2] - xx[N-1])

	for i in range(1,N-1):
		dxdt[i] = cx*(xx[i-1]-2*xx[i]+xx[i+1])
		
	return dxdt

def XY_stationary_explicit(y,t, *args):
	""" explicit RHS with 2nd order Neumann boundary conditions
	fast version using weave"""
	
	F_params = args[0]
	G_params = args[1]
	F_func = F_params[0]
	args_f = F_params[1]
	G_func = G_params[0]
	args_g = G_params[1]

	Dx = args[2]
	Dy = args[3]
	
	N = args[4]
	H = args[5]
	L = args[6]
	
	dxydt = zeros(2*N)
	xx = y[:N]
	yy = y[N:]

	L2 = (2*L)**2
	cx = Dx/(L2*H**2)
	cy = Dy/(L2*H**2)

	dxdt = zeros(N)
	dydt = zeros(N)
	RHS_x = zeros(N)
	RHS_y = zeros(N)
	
	RHS_x = F_func(xx, yy, args_f)
	RHS_y = G_func(xx, yy, args_g)

	dxdt[0] = 2*cx*(xx[1] - xx[0]) + RHS_x[0]
	dxdt[N-1] = 2*cx*(xx[N-2] - xx[N-1]) + RHS_x[N-1]

	dydt[0] = 2*cy*(yy[1] - yy[0]) + RHS_y[0]
	dydt[N-1] = 2*cy*(yy[N-2] - yy[N-1]) + RHS_y[N-1]
	
	expr1 = "dxdt[1:N-1] = cx*(xx[:N-2]-2*xx[1:N-1]+xx[2:]) + RHS_x[1:N-1]"
	expr2 = "dydt[1:N-1] = cy*(yy[:N-2]-2*yy[1:N-1]+yy[2:]) + RHS_y[1:N-1]"
	weave.blitz(expr1, check_size=0)
	weave.blitz(expr2, check_size=0)

	for i in range(N):
		dxydt[i] = dxdt[i]
		dxydt[N+i] = dydt[i]

	return dxydt

def XY_stationary_explicit_extended(y,t, *args):
	""" explicit RHS with 2nd order Neumann boundary conditions"""
	F_params = args[0]
	G_params = args[1]
	F_func = F_params[0]
	args_f = F_params[1]
	G_func = G_params[0]
	Y2 = G_params[1][0]
	args_g = G_params[1][1:]

	Dx = args[2]
	Dy = args[3]
	
	N = args[4]
	H = args[5]
	L = args[6]
	
	dxydt = zeros(2*N)
	xx = y[:N]
	yy = y[N:]

	L2 = (2*L)**2
	cx = Dx/(L2*H**2)
	cy = Dy/(L2*H**2)

	dxdt = zeros((N,1))
	dydt = zeros((N,1))

	dxdt[0] = 2*cx*(xx[1] - xx[0]) + F_func(xx[0], yy[0], args_f)
	dxdt[N-1] = 2*cx*(xx[N-2] - xx[N-1]) + F_func(xx[N-1], yy[N-1], args_f)

	dydt[0] = 2*cy*(yy[1] - yy[0]) + G_func(xx[0], yy[0], [Y2[0]]+args_g)
	dydt[N-1] = 2*cy*(yy[N-2] - yy[N-1]) + G_func(xx[N-1], yy[N-1], [Y2[N-1]]+args_g)

	for i in range(1,N-1):
		dxdt[i] = cx*(xx[i-1]-2*xx[i]+xx[i+1]) + F_func(xx[i], yy[i], args_f)
		dydt[i] = cy*(yy[i-1]-2*yy[i]+yy[i+1]) + G_func(xx[i], yy[i], [Y2[i]]+args_g)
	
	for i in range(N):
		dxydt[i] = dxdt[i]
		dxydt[N+i] = dydt[i]

	return dxydt

if __name__=="__main__":
	
	def F_bruss(X, Y, a, b):
		return a - b*X + X**2*Y - X
	def G_bruss(X, Y, b):
		return b*X - X**2*Y
	
	N = 100
	L = 1.0 # length of the domain
	H = 2*L/(N-1) # N grid points 
	x = arange(-L,L+H,H)

	Tend = 20. # integration time
	Dt = .05 # time step size, arbitrary for nonlinear problems
	Tsteps = arange(0,Tend,Dt)

	# non-dimensionalized parameters
	n = 20. # diffusion coefficient ratio, in the v equation
	Dx = 0.1
	Dy = Dx*n
	a = 8.
	b = 10.

	# Lacalli parameters
	k_1 = b - 1;
	k_2 = a**2
	k_3 = -b
	k_4 = -a**2
	k_cr = sqrt(-k_2*k_3)
	k_1p = k_1/k_cr
	k_4p = k_4/k_cr

	# Lacalli-Turing plot
	K_1p = arange(0.1,2,0.01)
	condition_a = n*K_1p - 2*sqrt(n)
	condition_b = K_1p - (n+1)/sqrt(n)
	condition_c = K_1p - 2
	condition_d = -K_1p
	condition_e = K_1p - 4*sqrt(n)/(n+1)
	condition_f = -1./K_1p

	#figure(1)
	#plot(K_1p,condition_a,K_1p,condition_b,K_1p,condition_c,K_1p, \
	#condition_d, K_1p,condition_e,K_1p,condition_f)
	#axis([0,1.2,-7,0])
	#plot(k_1p,k_4p,'*')
	#show()

	# initial conditions
	X_homog = a
	Y_homog = b/a

	XY_0 = zeros(2*N)
	XY_0[:N] = (cos(2*pi*x) + 1 + 0.01*random.randn(N))*X_homog/1.5 + 1.5;
	XY_0[N:] = (1 - cos(2*pi*x) + 0.01*random.randn(N))*Y_homog/6 + 1;
	
	args = (F_bruss, G_bruss, a, b, Dx, Dy, N, H, L)
	bla = integrate.odeint(XY_stationary_explicit, XY_0, Tsteps, args);
	#XX0 = XY(:,1:N);
	#YY0 = XY(:,N+1:2*N);
	figure(2)
	for i in range(0,len(bla),len(bla)/20):
		plot(bla[i][:N])

	Tsteps_plot = range(0,len(Tsteps),int(len(Tsteps)/N))
	#figure(3)
	three_d = surf(Tsteps[Tsteps_plot], x, bla[Tsteps_plot,:N],extent=[0,2,0,1,0,1])
	show()
