from numpy import sin, cos, sqrt, zeros, random, arange, pi
from scipy import integrate
from pylab import figure, plot, show, axis, ceil
from mayavi.mlab import surf, axes

# *** start ipython as ipython --gui=wx ***

# finite difference discretizations for various systems   
def XY_stationary_explicit(y, t, *args):
	"""Solve explicitly a 1D coupled system of two Brusselators"""
	N = args[0]
	H1 = args[1]
	L1 = args[2]
	a1 = args[3]
	b1 = args[4]
	H2 = args[5]
	L2 = args[6]
	a2 = args[7]
	b2 = args[8]
	n1 = args[9]
	E = args[10]

	dxydt = zeros(4*N)
	xx1 = y[:N]
	yy1 = y[N:2*N]
	xx2 = y[2*N:3*N]
	yy2 = y[3*N:4*N]

	cx1 = Dx/(2*L1*H1)**2
	cy1 = cx1*Dy/Dx
	cx2 = Dx/(2*L2*H2)**2
	cy2 = cx2*Dy/Dx

	dx1dt = zeros((N,1))
	dy1dt = zeros((N,1))
	dx2dt = zeros((N,1))
	dy2dt = zeros((N,1))

	dx1dt[0] = 2*cx1*(xx1[1] - xx1[0]) + F_bruss1(xx1[0], yy1[0], a1, b1)
	dx1dt[N-1] = 2*cx1*(xx1[N-2] - xx1[N-1]) + F_bruss1(xx1[N-1], yy1[N-1], a1, b1)

	dy1dt[0] = 2*cy1*(yy1[1] - yy1[0]) + G_bruss1(xx1[0], yy1[0], b1)
	dy1dt[N-1] = 2*cy1*(yy1[N-2] - yy1[N-1]) + G_bruss1(xx1[N-1], yy1[N-1], b1)
	
	dx2dt[0] = 2*cx2*(xx2[1] - xx2[0]) + F_bruss2(xx2[0], yy2[0], yy1[0], a2, n1, E)
	dx2dt[N-1] = 2*cx2*(xx2[N-2] - xx2[N-1]) + F_bruss2(xx2[N-1], yy2[N-1], yy1[N-1], a2, n1, E)

	dy2dt[0] = 2*cy2*(yy2[1] - yy2[0]) + G_bruss2(xx2[1], yy2[0], yy1[0], n1, E)
	dy2dt[N-1] = 2*cy2*(yy2[N-2] - yy2[N-1]) + G_bruss2(xx2[N-1], yy2[N-1], yy1[N-1], n1, E)

	for i in range(1,N-1):
		dx1dt[i] = cx1*(xx1[i-1]-2*xx1[i]+xx1[i+1]) + F_bruss1(xx1[i], yy1[i], a1, b1)
		dy1dt[i] = cy1*(yy1[i-1]-2*yy1[i]+yy1[i+1]) + G_bruss1(xx1[i], yy1[i], b1)
		dx2dt[i] = cx2*(xx2[i-1]-2*xx2[i]+xx2[i+1]) + F_bruss2(xx2[i], yy2[i], yy1[i], a2, n1, E)
		dy2dt[i] = cy2*(yy2[i-1]-2*yy2[i]+yy2[i+1]) + G_bruss2(xx2[i], yy2[i], yy1[i], n1, E)
		
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
	""" explicit RHS with 2nd order Neumann boundary conditions"""
	F_func = args[0]
	G_func = args[1]
	a = args[2]
	b = args[3]
	Dx = args[4]
	Dy = args[5]
	
	N = args[6]
	H = args[7]
	L = args[8]
	
	dxydt = zeros(2*N)
	xx = y[:N]
	yy = y[N:]

	L2 = (2*L)**2
	cx = Dx/(L2*H**2)
	cy = Dy/(L2*H**2)

	dxdt = zeros((N,1))
	dydt = zeros((N,1))

	dxdt[0] = 2*cx*(xx[1] - xx[0]) + F_func(xx[0], yy[0], a, b)
	dxdt[N-1] = 2*cx*(xx[N-2] - xx[N-1]) + F_func(xx[N-1], yy[N-1], a, b)

	dydt[0] = 2*cy*(yy[2] - yy[1]) + G_func(xx[1], yy[1], b)
	dydt[N-1] = 2*cy*(yy[N-2] - yy[N-1]) + G_func(xx[N-1], yy[N-1], b)

	for i in range(1,N-1):
		dxdt[i] = cx*(xx[i-1]-2*xx[i]+xx[i+1]) + F_func(xx[i], yy[i], a, b)
		dydt[i] = cy*(yy[i-1]-2*yy[i]+yy[i+1]) + G_func(xx[i], yy[i], b)
	
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
