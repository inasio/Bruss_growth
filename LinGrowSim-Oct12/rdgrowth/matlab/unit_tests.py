from numpy import sin, cos, sqrt, zeros, random, arange, pi, exp
from scipy import integrate
from pylab import figure, plot, show, axis, ceil, norm, semilogy, loglog, semilogx
from mayavi.mlab import surf, axes
from bruss_1d_stuff import diffusion_stationary_explicit

# *** start ipython as ipython --gui=wx ***

def exact_soln(t_steps, *args):
	"""exact solution to the heat equation"""
	x = args[0]
	L = args[1]
	exact_u = zeros((len(t_steps), len(x)))
	for i in range(len(t_steps)):
		exact_u[i,:] = sin(pi*x/(2*L))*exp(-(pi/(2*L))**2*t_steps[i])
	return exact_u
		
if __name__=="__main__":
	
	NN = [40,80,160,320,640]
	for N in NN:
		Tend = 0.5 # integration time
		Tsteps = arange(0, Tend, Tend/100.)

		L = 1.0 
		H = 2*L/(N-1) # N grid points 
		x = arange(-L,L+H,H)
		
		ode_params = (N, H, L)

		XY_0 = zeros(N)
		XY_0[:N] = sin(pi*x/(2*L));

		bla = integrate.odeint(diffusion_stationary_explicit, XY_0, Tsteps, args = ode_params);
		u = exact_soln(Tsteps, x, L)
		
		Error = norm(bla[-1,:] - u[-1,:])
		print 'error = ',Error
		figure(1)
		loglog(N,Error,'o')
	y_test_data = [(i)**(-2) for i in NN]
	loglog(NN, y_test_data,'--')	
		
	show()
