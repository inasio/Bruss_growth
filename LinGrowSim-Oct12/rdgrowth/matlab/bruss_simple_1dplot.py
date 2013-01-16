from numpy import sin, cos, sqrt, zeros, random, arange, pi
from scipy import integrate
import pylab as pb

# nonlinear function definitions   
def F_bruss(X,Y,a,b,c,d):
	return a - b*X + c*X**2*Y - d*X

def G_bruss(X,Y,b,c):
	return b*X - c*X**2*Y

# explicit RHS with 2nd order Neumann boundary conditions
def XY_stationary_explicit(y,t = 0):

	dxydt = zeros(2*N)
	xx = y[:N]
	yy = y[N:]

	L2 = (2*L)**2
	cx = Dx/(L2*H**2)
	cy = Dy/(L2*H**2)

	dxdt = zeros((N,1))
	dydt = zeros((N,1))

	dxdt[0] = cx*(xx[1] - xx[0]) + F_bruss(xx[0],yy[0],a,b,c,d)
	dxdt[N-1] = cx*(xx[N-2] - xx[N-1]) + F_bruss(xx[N-1],yy[N-1],a,b,c,d)

	dydt[0] = cy*(yy[2] - yy[1]) + G_bruss(xx[1],yy[1],b,c)
	dydt[N-1] = cy*(yy[N-2] - yy[N-1]) + G_bruss(xx[N-1],yy[N-1],b,c)

	for i in range(1,N-1):
		dxdt[i] = cx*(xx[i-1]-2*xx[i]+xx[i+1]) + F_bruss(xx[i],yy[i],a,b,c,d)
		dydt[i] = cy*(yy[i-1]-2*yy[i]+yy[i+1]) + G_bruss(xx[i],yy[i],b,c)
	
	for i in range(N):
		dxydt[i] = dxdt[i]
		dxydt[N+i] = dydt[i]

	return dxydt

N = 60
L = 1.2 # length of the domain
H = 2*L/(N-1) # 100 grid points 
x = arange(-L,L+H,H)
n = 20
Dx = 0.005
Dy = n*Dx # diffusion coefficient ratio, in the v equation

Tend = 10. # integration time
Dt = .005 # time step size, arbitrary for nonlinear problems

a = 0.05
b = 0.5
c = 1.8
d = 0.05

# non-dimensionalization stuff
Dx = Dx/d
Dy = Dy/d
a = a*c/(d*sqrt(d))
b = b/d
c = 1.
d = 1.

# Lacalli parameters
k_1 = b-d;
k_2 = a**2*c/d**2
k_3 = -b
k_4 = -a**2*c/d**2
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
#pb.figure(1)
#pb.plot(K_1p,condition_a,K_1p,condition_b,K_1p,condition_c,K_1p, \
#condition_d,
  #K_1p,condition_e,K_1p,condition_f)
#pb.axis([0,1.2,-7,0])
#pb.plot(k_1p,k_4p,'*')
#pb.show()

# initial conditions
X_homog = a/d
Y_homog = b*d/(a*c)

XY_0 = zeros(2*N)
XY_0[:N] = (cos(2*pi*x) + 1 + 0.01*random.randn(N))*X_homog/1.5 + 1.5;
XY_0[N:] = (1 - cos(2*pi*x) + 0.01*random.randn(N))*Y_homog/6 + 1;

bla = integrate.odeint(XY_stationary_explicit, XY_0, arange(0,Tend,Dt));
#XX0 = XY(:,1:N);
#YY0 = XY(:,N+1:2*N);
for i in range(0,len(bla),len(bla)/20):
    pb.plot(bla[i][:N])
pb.show()
