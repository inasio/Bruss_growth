from numpy import sin, cos, sqrt, zeros, random, arange, pi, savez, load
from scipy import integrate
from pylab import figure, plot, show, axis, ceil
from mayavi.mlab import surf, axes
from bruss_1d_stuff import XY_stationary_explicit
import time

# *** start ipython as ipython --gui=wx ***

def F_bruss(X, Y, a, b):
	return a - b*X + X**2*Y - X
def G_bruss(X, Y, b):
	return b*X - X**2*Y

LL = arange(0.9,1.5,0.01)
for index, value in enumerate(LL):
	chrono_0 = time.time()
	L = LL[index]
	N = 200
	#L = 1.0 # length of the domain
	H = 2*L/(N-1) # N grid points 
	x = arange(-L,L+1e-6,H)

	Tend = 150. # integration time
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
	if index == 0:
		X_homog = a
		Y_homog = b/a

		XY_0 = zeros(2*N)
		XY_0[:N] = (cos(2*pi*x) + 1 + 0.01*random.randn(N))*X_homog/1.5 + 1.5
		XY_0[N:] = (1 - cos(2*pi*x) + 0.01*random.randn(N))*Y_homog/6 + 1
	else:
		XY_0 = bla[-1,:]+ 0.001*random.randn(2*N)

	args = (F_bruss, G_bruss, a, b, Dx, Dy, N, H, L)
	bla = integrate.odeint(XY_stationary_explicit, XY_0, Tsteps, args);

	# saving and loading stuff
	file_name = 'data/bruss_adim_files_' + str(index).zfill(4)
	savez(file_name,bla=bla, Tsteps=Tsteps, x=x, args=args)
	#test_save = load(file_name + '.npz')
	#x = test_save['x']
	#bla = test_save['bla']
	#Tsteps = test_save['Tsteps']

	figure(2)
	#for i in range(0,len(bla),len(bla)/20):
	plot(bla[-1][:N])

	Tsteps_plot = range(0,len(Tsteps),int(len(Tsteps)/N))
	#figure(3)
	#three_d = surf(Tsteps[Tsteps_plot], x, bla[Tsteps_plot,:N],extent=[0,2,0,1,0,1])
	print L, time.time() - chrono_0
show()
