from numpy import sin, cos, sqrt, zeros, random, arange, pi, savez, load
from scipy import integrate, interpolate
from pylab import figure, plot, show, axis, ceil
from mayavi.mlab import surf, axes
from bruss_1d_stuff import XY_stationary_explicit_extended
from time import time

# *** start ipython as ipython --gui=wx ***

def F_bruss(X, Y, args):
	a = args[0]
	b = args[1]
	return a - b*X + X**2*Y - X
def G_bruss(X, Y, args):
	b = args[0]
	return b*X - X**2*Y
def G_bruss_ext(X, Y, args):
	Y1 = args[0]
	n1 = args[1]
	E = args[2]
	#b = n1*Y1/(Y1 + E)
	b = 10
	return b*X - X**2*Y

clock_start = time()
N = 100
L = 1.0 # length of the domain
H = 2*L/(N-1) # N grid points 
x = arange(-L,L+1e-6,H)

Tend = 20. # integration time
Dt = .05 # time step size, arbitrary for nonlinear problems
Tsteps = arange(0,Tend,Dt)

# non-dimensionalized parameters
n = 20. # diffusion coefficient ratio, in the v equation
Dx = 0.1
Dy = Dx*n
a = 8.
b = 10.

n1 = 15.
E = 2.

# initial conditions

XY_0 = zeros(2*N)
filenum_init = '0030'
init_conditions = load('data/bruss_adim_files_'+filenum_init+'.npz')
XY_0_init = init_conditions['bla'][-1,:]
x_init = init_conditions['x']
x_init = x_init*x[-1]/x_init[-1]		#rescaling in case L_init <> L
XY_0[:N] = interpolate.interp1d(x_init, XY_0_init[:len(x_init)], bounds_error=False, fill_value=0., kind='cubic')(x)
XY_0[N:] = interpolate.interp1d(x_init, XY_0_init[len(x_init):], bounds_error=False, fill_value=0., kind='cubic')(x)

filenum_GM2 = '0059'
GM2_data = load('data/bruss_adim_files_'+filenum_GM2+'.npz')
x_GM2 = GM2_data['x']
Y_GM2 = GM2_data['bla'][-1,len(x_GM2):]
x_GM2 = x_GM2*x[-1]/x_GM2[-1]		#rescaling in case L_init <> L
Y2 = interpolate.interp1d(x_GM2, Y_GM2[:len(x_GM2)], bounds_error=False, fill_value=0., kind='cubic')(x)

F_params = [F_bruss, [a, b]]
#G_params = [G_bruss, [b]]
G_params = [G_bruss_ext, [Y2, n1, E]]
args = (F_params, G_params, Dx, Dy, N, H, L)

clock_pre_integration = time()
bla = integrate.odeint(XY_stationary_explicit_extended, XY_0, Tsteps, args);
clock_integration = time() - clock_pre_integration
clock_total = time() - clock_start

# saving and loading stuff
savez('bruss_adim_files',bla=bla, Tsteps=Tsteps, x=x)
test_save = load('bruss_adim_files.npz')

x = test_save['x']
bla = test_save['bla']
Tsteps = test_save['Tsteps']

figure(2)
for i in range(0,len(bla),len(bla)/20):
    plot(bla[i][:N])

Tsteps_plot = range(0,len(Tsteps),int(len(Tsteps)/N))
#figure(3)
three_d = surf(Tsteps[Tsteps_plot], x, bla[Tsteps_plot,N:],extent=[0,2,0,1,0,1])

print 't_load = ',clock_pre_integration
print 't_num = ',clock_integration
print 't_total = ', clock_total
show()
