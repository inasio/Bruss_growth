from numpy import sin, cos, sqrt, random, arange, pi, load, sort
from scipy import integrate, sparse, zeros, ones, hstack, vstack, array, linalg, diagflat
from pylab import figure, plot, show, axis, ceil
from mayavi.mlab import surf, axes
#from bruss_1d_stuff import F_bruss, G_bruss, XY_stationary_explicit

# *** start ipython as ipython --gui=wx ***

def F_bruss(X, Y, a, b):
	return a - (b+1)*X + X**2*Y
def G_bruss(X, Y, b):
	return b*X - X**2*Y

# explicit RHS with 2nd order Neumann boundary conditions
def XY_stationary_explicit(y, t, *args):
	"""Solve explicitly a 1D coupled system of two Brusselators"""
	N = args[0]
	H = args[1]
	L = args[2]
	a = args[3]
	b = args[4]

	dxydt = zeros(4*N)
	xx = y[:N]
	yy = y[N:2*N]
	

	cx = Dx/(2*L*H)**2
	cy = cx*Dy/Dx

	dxdt = zeros((N,1))
	dydt = zeros((N,1))

	dxdt[0] = 2*cx*(xx[1] - xx[0]) + F_bruss(xx[0], yy[0], a, b)
	dxdt[N-1] = 2*cx*(xx[N-2] - xx[N-1]) + F_bruss(xx[N-1], yy[N-1], a, b)

	dydt[0] = 2*cy*(yy[1] - yy[0]) + G_bruss(xx[0], yy[0], b)
	dydt[N-1] = 2*cy*(yy[N-2] - yy[N-1]) + G_bruss(xx[N-1], yy[N-1], b)
	
	for i in range(1,N-1):
		dxdt[i] = cx*(xx[i-1]-2*xx[i]+xx[i+1]) + F_bruss(xx[i], yy[i], a, b)
		dydt[i] = cy*(yy[i-1]-2*yy[i]+yy[i+1]) + G_bruss(xx[i], yy[i], b)
		
	for i in range(N):
		dxydt[i] = dxdt[i]
		dxydt[N+i] = dydt[i]
		
	return dxydt

def laplacian(*args):
	n = args[0]
	cu = args[1]
	cv = args[2]
	e = ones((1,n))
	data = vstack((e, -2*e, e))
	diags = array([-1,0,1])
	lap = sparse.spdiags(data, diags, n, n).todense()
	lap[0, 1] = 2
	lap[n-1, n-2] = 2
	LAP = zeros((2*n, 2*n))
	LAP[:n, :n] = cu*lap
	LAP[n:, n:] = cv*lap
	return LAP

def jacobian(*args):
	F_func = args[0]
	G_func = args[1]
	u = args[2]
	v = args[3]
	a = args[4]
	b = args[5]
	n = len(u)
	JAC = zeros((2*n,2*n))
	dfdu = (F_func(u + ones((1,n))*1e-4, v, a, b) - F_func(u, v, a, b))/1e-4
	dfdv = (F_func(u, v + ones((1,n))*1e-4, a, b) - F_func(u, v, a, b))/1e-4
	dgdu = (G_func(u + ones((1,n))*1e-4, v, b) - G_func(u, v, b))/1e-4
	dgdv = (G_func(u, v + ones((1,n))*1e-4, b) - G_func(u, v, b))/1e-4
	JAC[:n, :n] = diagflat(dfdu)
	JAC[:n, n:] = diagflat(dfdv)
	JAC[n:, :n] = diagflat(dgdu)
	JAC[n:, n:] = diagflat(dgdv)
	return JAC
	
if __name__=="__main__":

	#aa = laplacian(4,1,1)
	#print linalg.eigvals(aa)
	#temp = ones((4,1))
	#print jacobian(F_bruss, G_bruss, temp, temp, a, b)
	
	for num in range(60):
		file_name = 'data/bruss_adim_files_' + str(num).zfill(4)
		test_save = load(file_name + '.npz')
		x = test_save['x']
		bla = test_save['bla']
		Tsteps = test_save['Tsteps']
		args = test_save['args']
		F_func = args[0]
		G_func = args[1]
		a = args[2]
		b = args[3]
		Dx = args[4]
		Dy = args[5]
		N = args[6]
		H = args[7]
		L = args[8]
		u_stat = bla[-1,:N]
		v_stat = bla[-1,N:]
		Jac = jacobian(F_func, G_func, u_stat, v_stat, a, b)
		Lap = laplacian(N, Dx/(H*L)**2, Dy/(H*L)**2)
		Eigs = linalg.eigvals(Jac + Lap)
		max_eigs = sort(Eigs.real)[-10:]
		plot(L*ones((10,1)), max_eigs,'o')
		print num
	show()