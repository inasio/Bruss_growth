from numpy import sin, cos, sqrt, zeros, random, arange, pi
from scipy import integrate
from pylab import figure, plot, show, axis, ceil
from mayavi.mlab import surf, axes
#from bruss_1d_stuff import F_bruss, G_bruss, XY_stationary_explicit

# *** start ipython as ipython --gui=wx ***

def F_bruss(X,Y):
	return a - b*X + X**2*Y - X
def G_bruss(X,Y):
	return b*X - X**2*Y
# explicit RHS with 2nd order Neumann boundary conditions
def XY_stationary_explicit(y,t=0):
	"""Solve explicitly a 1D Brusselator system"""
	
	dxydt = zeros(2*N)
	xx = y[:N]
	yy = y[N:]

	L2 = (2*L)**2
	cx = Dx/(L2*H**2)
	cy = Dy/(L2*H**2)

	dxdt = zeros((N,1))
	dydt = zeros((N,1))

	dxdt[0] = 2*cx*(xx[1] - xx[0]) + F_bruss(xx[0],yy[0])
	dxdt[N-1] = 2*cx*(xx[N-2] - xx[N-1]) + F_bruss(xx[N-1],yy[N-1])

	dydt[0] = 2*cy*(yy[1] - yy[0]) + G_bruss(xx[0],yy[0])
	dydt[N-1] = 2*cy*(yy[N-2] - yy[N-1]) + G_bruss(xx[N-1],yy[N-1])

	for i in range(1,N-1):
		dxdt[i] = cx*(xx[i-1]-2*xx[i]+xx[i+1]) + F_bruss(xx[i],yy[i])
		dydt[i] = cy*(yy[i-1]-2*yy[i]+yy[i+1]) + G_bruss(xx[i],yy[i])
	
	for i in range(N):
		dxydt[i] = dxdt[i]
		dxydt[N+i] = dydt[i]

	return dxydt

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

bla = integrate.odeint(XY_stationary_explicit, XY_0, Tsteps);
#XX0 = XY(:,1:N);
#YY0 = XY(:,N+1:2*N);
figure(2)
for i in range(0,len(bla),len(bla)/20):
    plot(bla[i][:N])

Tsteps_plot = range(0,len(Tsteps),int(len(Tsteps)/N))
#figure(3)
three_d = surf(Tsteps[Tsteps_plot], x, bla[Tsteps_plot,:N],extent=[0,2,0,1,0,1])
show()
