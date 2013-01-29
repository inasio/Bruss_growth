from numpy import sin, cos, sqrt, zeros, random, arange, pi, exp, ones
from scipy import integrate, sparse, zeros
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

def convergence_test():
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
	
def eigenvalue_test():
	N = 100
	L = 1.0 # length of the domain
	H = 2*L/(N-1) # N grid points 
	x = arange(-L,L+H,H)
	u = sin(x)
	v = exp(-x)
	f = u**2*v
	g = u**2 + v**2
	dfdu = sparse.spdiags(2*u*v,0,N,N)
	dfdv = sparse.spdiags(u**2,0,N,N)
	dgdu = sparse.spdiags(2*u,0,N,N)
	dgdv = sparse.spdiags(2*v,0,N,N)
	Lap = zeros((2*N, 2*N))
	Lap[:N, :N] = dfdu
	Lap[:N, N:] = dgdu
	print Lap.todense()
	
def Jacobian_test():
	def f_func(u,v):
		return u**2*v
	def g_func(u,v):
		return u**2 + v**2
	N = 30
	L = 1.0 # length of the domain
	H = 2*L/(N-1) # N grid points 
	x = arange(-L,L+H,H)
	u = sin(x)
	v = exp(-x)
	f = u**2*v
	g = u**2 + v**2
	dfdu = 2*u*v
	dfdv = u**2
	dgdu = 2*u
	dgdv = 2*v
	dfdu_num = (f_func(u + 1e-4*ones(N), v) - f_func(u, v))/1e-4
	dfdv_num = (f_func(u, v + 1e-4*ones(N)) - f_func(u, v))/1e-4
	dgdu_num = (g_func(u + 1e-4*ones(N), v) - g_func(u, v))/1e-4
	dgdv_num = (g_func(u, v + 1e-4*ones(N)) - g_func(u, v))/1e-4

	a11 = sparse.spdiags(dfdu,0,N,N)
	a12 = sparse.spdiags(dfdv,0,N,N)
	a21 = sparse.spdiags(dgdu,0,N,N)
	a22 = sparse.spdiags(dgdv,0,N,N)
	a11_num = sparse.spdiags(dfdu_num,0,N,N)
	a12_num = sparse.spdiags(dfdv_num,0,N,N)
	a21_num = sparse.spdiags(dgdu_num,0,N,N)
	a22_num = sparse.spdiags(dgdv_num,0,N,N)
	
	Lap = zeros((2*N, 2*N))
	Lap_num = zeros((2*N, 2*N))
	Lap[:N, :N] = a11.todense()
	Lap[:N, N:] = a12.todense()
	Lap[N:, :N] = a21.todense()
	Lap[N:, N:] = a22.todense()
	Lap_num[:N, :N] = a11_num.todense()
	Lap_num[:N, N:] = a12_num.todense()
	Lap_num[N:, :N] = a21_num.todense()
	Lap_num[N:, N:] = a22_num.todense()
	
	plot(x,dgdu,x,dgdu_num); show()
	
		
if __name__=="__main__":
	
	#convergence_test()
	#eigenvalue_test()
	Jacobian_test()
	
	
	#print Lap