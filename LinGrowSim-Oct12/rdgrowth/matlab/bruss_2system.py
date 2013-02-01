from numpy import sin, cos, sqrt, zeros, random, arange, pi
from scipy import integrate
from pylab import figure, plot, show, axis, ceil
from mayavi.mlab import surf, axes
from bruss_1d_stuff import XY2_stationary_explicit

# *** start ipython as ipython --gui=wx ***

def F_bruss1(X, Y, args):
	a = args[0]
	b = args[1]
	return a - (b+1)*X + X**2*Y
def G_bruss1(X, Y, args):
	b = args[0]
	return b*X - X**2*Y
def F_bruss2(X, Y, args):
	Y1 = args[0]
	a = args[1]
	n1 = args[2]
	E = args[3]
	b = n1*Y1/(Y1 + E)
	return a - (b+1)*X + X**2*Y
def G_bruss2(X, Y, args):
	Y1 = args[0]
	n1 = args[1]
	E = args[2]
	b = n1*Y1/(Y1 + E)
	return b*X - X**2*Y

if __name__=="__main__":

	N = 100
	Tend = 40. # integration time
	Dt = .05 # time step size, arbitrary for nonlinear problems
	Tsteps = arange(0,Tend,Dt)
	n = 20. # diffusion coefficient ratio, for the v equation
	Dx = 0.1
	Dy = Dx*n

	L1 = 0.9 
	H1 = 2*L1/(N-1) # N grid points 
	x1 = arange(-L1,L1+1e-6,H1)
	a1 = 8.
	b1 = 10.

	L2 = 1.4
	H2 = 2*L2/(N-1) # N grid points 
	x2 = arange(-L2,L2+1e-6,H2)
	a2 = 8.
	b2 = 10.
	
	n1 = 15.
	E = 2.
	F1_params = [F_bruss1, [a1, b1]]
	G1_params = [G_bruss1, [b1]]
	F2_params = [F_bruss2, [a2, n1, E]]
	G2_params = [G_bruss2, [n1, E]]
	ode_params = (Dx, Dy, N, H1, L1, H2, L2, F1_params, G1_params, F2_params, G2_params)

	# initial conditions
	X1_homog = a1
	Y1_homog = b1/a1
	X2_homog = a2
	Y2_homog = b2/a2

	XY_0 = zeros(4*N)
	XY_0[:N] = (cos(2*pi*x1) + 1 + 0.01*random.randn(N))*X1_homog/1.5 + 1.5;
	XY_0[N:2*N] = (1 - cos(2*pi*x1) + 0.01*random.randn(N))*Y1_homog/6 + 1;
	XY_0[2*N:3*N] = (cos(2*pi*x2) + 1 + 0.01*random.randn(N))*X2_homog/1.8 + 1.5;
	XY_0[3*N:4*N] = (1 - cos(2*pi*x2) + 0.01*random.randn(N))*Y2_homog/6 + 1;

	bla = integrate.odeint(XY2_stationary_explicit, XY_0, Tsteps, args = ode_params);
	#XX0 = XY(:,1:N);
	#YY0 = XY(:,N+1:2*N);
	figure(2)
	for i in range(0,len(bla),len(bla)/20):
		plot(bla[i][2*N:3*N])
	figure(3)
	for i in range(0,len(bla),len(bla)/20):
		plot(bla[i][3*N:4*N])

	Tsteps_plot = range(0,len(Tsteps),int(len(Tsteps)/N))
	#figure(3)
	three_d = surf(Tsteps[Tsteps_plot], x1, bla[Tsteps_plot,2*N:3*N],extent=[0,2,0,1,0,1])
	show()
