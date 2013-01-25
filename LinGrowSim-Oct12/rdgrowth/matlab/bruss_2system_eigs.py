from numpy import sin, cos, sqrt, zeros, random, arange, pi
from scipy import integrate
from pylab import figure, plot, show, axis, ceil
from mayavi.mlab import surf, axes
#from bruss_1d_stuff import F_bruss, G_bruss, XY_stationary_explicit

# *** start ipython as ipython --gui=wx ***

def F_bruss1(X, Y, a, b):
	return a - (b+1)*X + X**2*Y
def G_bruss1(X, Y, b):
	return b*X - X**2*Y
def F_bruss2(X, Y, Y1, a, n1, E):
	b = n1*Y1/(Y1 + E)
	return a - (b+1)*X + X**2*Y
def G_bruss2(X, Y, Y1, n1, E):
	b = n1*Y1/(Y1 + E)
	return b*X - X**2*Y
# explicit RHS with 2nd order Neumann boundary conditions
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

if __name__=="__main__":

	N = 100
	Tend = 10. # integration time
	Dt = .05 # time step size, arbitrary for nonlinear problems
	Tsteps = arange(0,Tend,Dt)
	n = 20. # diffusion coefficient ratio, for the v equation
	Dx = 0.1
	Dy = Dx*n

	L1 = 1.0 
	H1 = 2*L1/(N-1) # N grid points 
	x1 = arange(-L1,L1+H1,H1)
	a1 = 8.
	b1 = 10.

	L2 = 1.0
	H2 = 2*L2/(N-1) # N grid points 
	x2 = arange(-L2,L2+H2,H2)
	a2 = 8.
	b2 = 10.
	
	n1 = 25.
	E = 2.
	ode_params = (N, H1, L1, a1, b1, H2, L2, a2, b2, n1, E)

	# Lacalli parameters
	k_1 = b1 - 1;
	k_2 = a1**2
	k_3 = -b1
	k_4 = -a1**2
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
	X1_homog = a1
	Y1_homog = b1/a1
	X2_homog = a2
	Y2_homog = b2/a2

	XY_0 = zeros(4*N)
	XY_0[:N] = (cos(2*pi*x1) + 1 + 0.01*random.randn(N))*X1_homog/1.5 + 1.5;
	XY_0[N:2*N] = (1 - cos(2*pi*x1) + 0.01*random.randn(N))*Y1_homog/6 + 1;
	XY_0[2*N:3*N] = (cos(2*pi*x2) + 1 + 0.01*random.randn(N))*X2_homog/1.5 + 1.5;
	XY_0[3*N:4*N] = (1 - cos(2*pi*x2) + 0.01*random.randn(N))*Y2_homog/6 + 1;

	bla = integrate.odeint(XY_stationary_explicit, XY_0, Tsteps, args = ode_params);
	#XX0 = XY(:,1:N);
	#YY0 = XY(:,N+1:2*N);
	figure(2)
	for i in range(0,len(bla),len(bla)/20):
		plot(bla[i][:N])

	Tsteps_plot = range(0,len(Tsteps),int(len(Tsteps)/N))
	#figure(3)
	three_d = surf(Tsteps[Tsteps_plot], x1, bla[Tsteps_plot,2*N:3*N],extent=[0,2,0,1,0,1])
	show()
